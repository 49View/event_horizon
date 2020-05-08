//
// Created by Dado on 09/12/2019.
//

#include "mongo.hpp"
#include <core/file_manager.h>
#include <core/string_util.h>
#include <core/http/webclient.h>

Mongo::Mongo(const DBConnection &_cs) {
    auto completeConnectionString =
            "mongodb://" + _cs.path + "/" + _cs.defaultDatabase + "?replicaSet=" +
            _cs.replicaSet;
    conn = mongocxx::client{mongocxx::uri{completeConnectionString}};
    if (!_cs.defaultDatabase.empty()) {
        db = conn[_cs.defaultDatabase];
    }
}

MongoBucket Mongo::useBucket(const std::string &_bucketName) const {
    return MongoBucket{db, _bucketName};
}

MongoCollection Mongo::operator[](const std::string &_collName) const {
    return MongoCollection{db[_collName]};
}

mongocxx::change_stream MongoCollection::watch() {
    return collection.watch();
}

std::optional<uint8_p> Mongo::fileDownloadWithId(MongoBucket &bucket, const std::string &_id) {

    auto downloadStream = bucket().open_download_stream(MongoObjectId{strview{_id}}());
    auto buffer = make_uint8_p(downloadStream.file_length());
    auto ret = downloadStream.read(buffer.first.get(), downloadStream.file_length());
    if (ret == static_cast<size_t >(downloadStream.file_length())) {
        return buffer;
    }
    return std::nullopt;
}

uint8_p
Mongo::fileDownload(MongoBucket &bucket, const MongoObjectId &_id, std::function<void(uint8_p &&)> callback) {
    auto downloadStream = bucket().open_download_stream(_id());
    auto buffer = make_uint8_p(downloadStream.file_length());
    downloadStream.read(buffer.first.get(), downloadStream.file_length());
    return buffer;
}

std::string Mongo::fileDownload(MongoBucket &bucket, const MongoObjectId &_id, const std::string &filename,
                                std::function<void(const std::string &)> callback) {
    auto downloadStream = bucket().open_download_stream(_id());
    auto buffer = make_uint8_p(downloadStream.file_length());
    auto ret = static_cast<size_t >(downloadStream.read(buffer.first.get(), downloadStream.file_length()));
    if (ret == static_cast<size_t >(downloadStream.file_length())) {
        FM::writeLocalFile(filename, reinterpret_cast<const char *>(buffer.first.get()), buffer.second, true);
        return filename;
    }
    return "";
}

MongoFileUpload
Mongo::fileUpload(MongoBucket &bucket, const std::string &filename, uint8_p &&buffer, MongoDocumentValue doc,
                  std::function<void(const std::string &)> callback) {
    mongocxx::options::gridfs::upload options{};
    auto uploadStream = bucket().open_upload_stream(filename, options.metadata(doc.view()));
    uploadStream.write(buffer.first.get(), buffer.second);
    return MongoFileUpload{uploadStream.close()};
}

MongoFileUpload
Mongo::fileUpload(MongoBucket &bucket, const std::string &filename, const SerializableContainer &buffer,
                  MongoDocumentValue doc,
                  std::function<void(const std::string &)> callback) {
    mongocxx::options::gridfs::upload options{};
    auto uploadStream = bucket().open_upload_stream(filename, options.metadata(doc.view()));
    uploadStream.write(buffer.data(), buffer.size());
    return MongoFileUpload{uploadStream.close()};
}

MongoDocumentValue Mongo::FSMetadata(const std::string &group, strview project, strview uname,
                                     strview uemail,
                                     const std::string &contentType,
                                     const std::string &md5, const std::string &thumb,
                                     const ResourceDependencyDict &deps) {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::sub_array;
    using bsoncxx::builder::basic::make_document;

    auto builder = bsoncxx::builder::basic::document{};

    try {
        builder.append(
                kvp( "group", group ),
                kvp( "project", project ),
                kvp( "username", uname ),
                kvp( "useremail", uemail ),
                kvp( "contentType", contentType ),
                kvp( "hash", md5 ),
                kvp( "thumb", thumb ),
                kvp( "deps", [&]( sub_array sa ) {
                    for ( const auto& dep : deps ) {
                        sa.append( make_document(
                                kvp( "key", dep.first ),
                                kvp( "value", [&]( sub_array sa2 ) {
                                    for ( const auto& depv : dep.second ) {
                                        sa2.append( depv );
                                    }
                                } ))
                        );
                    }
                } )
        );

    } catch ( const std::exception& e ) {
        LOGRS( e.what() );
    }
    return MongoDocumentValue{ builder.extract() };
}

std::string Mongo::insertEntityFromAsset(const StreamChangeMetadata &meta) {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::sub_array;

    auto builder = bsoncxx::builder::basic::document{};
    auto timeNow = std::chrono::system_clock::now();
    builder.append(
            kvp("group", meta.group),
            kvp("fsid", meta.id()),
            kvp("name", meta.filename),
            kvp("project", meta.project),
            kvp("isPublic", true),
            kvp("isRestricted", false),
            kvp("contentType", meta.contentType),
            kvp("hash", meta.hash),
            kvp("thumb", meta.thumb),
            kvp("lastUpdatedDate", bsoncxx::types::b_date{timeNow}),
            kvp("creationDate", bsoncxx::types::b_date{timeNow}),
            kvp("tags", [&](sub_array sa) {
                for (const auto &tag : split_tags(std::string(meta.filename))) {
                    sa.append(toLower(tag));
                }
            }),
            kvp("creator", bsoncxx::builder::basic::make_document(
                    kvp("username", meta.username),
                    kvp("useremail", meta.useremail)
            )),
            kvp("deps", meta.deps.get_value())
    );

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db["entities"].insert_one(builder.view());
    if (result ) {
        auto dbc = db["entities"].find_one(bsoncxx::builder::stream::document{} << "_id" << (*result).inserted_id()
                                                                                << bsoncxx::builder::stream::finalize);
        return bsoncxx::to_json(dbc->view());
    }

    return "";
}

void Mongo::insertDaemonCrashLog( const std::string& crash, const std::string& username ) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::sub_array;

    auto builder = bsoncxx::builder::basic::document{};
    auto timeNow = std::chrono::system_clock::now();
    builder.append(
            kvp("crash", crash),
            kvp("username", username),
            kvp("lastUpdatedDate", bsoncxx::types::b_date{timeNow}),
            kvp("creationDate", bsoncxx::types::b_date{timeNow})
    );

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db["daemon_crashes"].insert_one(builder.view());
}

void MongoBucket::deleteAll() {
    bucketFiles.delete_many({});
    bucketChunks.delete_many({});
}

MongoBucket::MongoBucket(const mongocxx::database &db, const std::string &_bucketName) {
    mongocxx::options::gridfs::bucket options;
    options.bucket_name(_bucketName);
    bucket = db.gridfs_bucket(options);
    bucketFiles = db[_bucketName + ".files"];
    bucketChunks = db[_bucketName + ".chunks"];
}
