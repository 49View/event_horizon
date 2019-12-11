//
// Created by Dado on 09/12/2019.
//

#include "mongo.hpp"
#include <core/file_manager.h>
#include <core/string_util.h>

Mongo::Mongo( const DBConnection& _cs ) {
    auto host = _cs.host;
    auto port = _cs.ports.empty() && _cs.port == 0 ? 27017 : _cs.port;

    auto completeConnectionString =
            "mongodb://" + host + ":" + std::to_string( port ) + "/" + _cs.defaultDatabase + "?replicaSet=" +
            _cs.replicaSet;
    conn = mongocxx::client{ mongocxx::uri{ completeConnectionString }};
    if ( !_cs.defaultDatabase.empty()) {
        db = conn[_cs.defaultDatabase];
    }
}

MongoBucket Mongo::useBucket( const std::string& _bucketName ) const {
    mongocxx::options::gridfs::bucket b;
    b.bucket_name( _bucketName );
    return MongoBucket{ db.gridfs_bucket( b ) };
}

MongoCollection Mongo::operator[]( const std::string& _collName ) const {
    return MongoCollection{ db[_collName] };
}

mongocxx::change_stream MongoCollection::watch() {
    return collection.watch();
}

std::optional<uint8_p> Mongo::fileDownloadWithId( MongoBucket& bucket, const std::string& _id ) {

    auto downloadStream = bucket().open_download_stream( MongoObjectId{ strview{ _id }}());
    auto buffer = make_uint8_p( downloadStream.file_length());
    auto ret = downloadStream.read( buffer.first.get(), downloadStream.file_length());
    if ( ret == downloadStream.file_length()) {
        return buffer;
    }
    return std::nullopt;
}

void Mongo::fileDownload( MongoBucket& bucket, const MongoObjectId& _id, std::function<void( uint8_p&& )> callback ) {
    auto downloadStream = bucket().open_download_stream( _id());
    auto buffer = make_uint8_p( downloadStream.file_length());
    auto ret = downloadStream.read( buffer.first.get(), downloadStream.file_length());
    if ( ret == downloadStream.file_length()) {
        if ( callback ) callback( std::move( buffer ));
    }
}

void Mongo::fileDownload( MongoBucket& bucket, const MongoObjectId& _id, const std::string& filename,
                          std::function<void( const std::string& )> callback ) {
    auto downloadStream = bucket().open_download_stream( _id());
    auto buffer = make_uint8_p( downloadStream.file_length());
    auto ret = downloadStream.read( buffer.first.get(), downloadStream.file_length());
    if ( ret == downloadStream.file_length()) {
        FM::writeLocalFile( filename, reinterpret_cast<const char *>(buffer.first.get()), buffer.second, true );
        if ( callback ) callback( filename );
    }
}

MongoFileUpload
Mongo::fileUpload( MongoBucket& bucket, const std::string& filename, uint8_p&& buffer, MongoDocumentValue doc,
                   std::function<void( const std::string& )> callback ) {
    mongocxx::options::gridfs::upload options{};
    auto uploadStream = bucket().open_upload_stream( filename, options.metadata( doc.view()));
    uploadStream.write( buffer.first.get(), buffer.second );
    return MongoFileUpload{ uploadStream.close() };
}

MongoFileUpload
Mongo::fileUpload( MongoBucket& bucket, const std::string& filename, const SerializableContainer& buffer,
                   MongoDocumentValue doc,
                   std::function<void( const std::string& )> callback ) {
    mongocxx::options::gridfs::upload options{};
    auto uploadStream = bucket().open_upload_stream( filename, options.metadata( doc.view()));
    uploadStream.write( buffer.data(), buffer.size());
    return MongoFileUpload{ uploadStream.close() };
}

MongoDocumentValue Mongo::FSMetadata( const std::string& group, strview project, strview uname,
                                      strview uemail,
                                      const std::string& md5, const std::string& thumb,
                                      const ResourceDependencyDict& deps ) {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::sub_array;
    using bsoncxx::builder::basic::make_document;

    auto builder = bsoncxx::builder::basic::document{};
    builder.append(
            kvp( "group", group ),
            kvp( "project", project ),
            kvp( "username", uname ),
            kvp( "useremail", uemail ),
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

    return MongoDocumentValue{ builder.extract() };
}

void Mongo::insertEntityFromAsset( const StreamChangeMetadata& meta ) {

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::sub_array;

    auto builder = bsoncxx::builder::basic::document{};
    builder.append(
            kvp( "group", meta.group ),
            kvp( "fsid", meta.id()),
            kvp( "name", meta.filename ),
            kvp( "project", meta.project ),
            kvp( "isPublic", true ),
            kvp( "isRestricted", false ),
            kvp( "username", meta.username ),
            kvp( "useremail", meta.useremail ),
            kvp( "hash", meta.md5 ),
            kvp( "thumb", meta.thumb ),
            kvp( "tags", [&]( sub_array sa ) {
                for ( const auto& tag : split_tags( std::string( meta.filename ))) {
                    sa.append( tag );
                }
            } ),
            kvp( "deps", meta.deps.get_value() )
    );

    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db["entities"].insert_one( builder.view());
}
