//
// Created by Dado on 09/12/2019.
//

#pragma once

#include <optional>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/gridfs/downloader.hpp>

#include <core/util.h>
#include <core/string_util.h>

#include "../no_sql.hpp"

using strview = bsoncxx::v_noabi::stdx::string_view;

class MongoBucket : public NoSqlStorage {
public:
    explicit MongoBucket( const mongocxx::database &db, const std::string &_bucketName );

    mongocxx::gridfs::bucket operator()() { return bucket; }

    void deleteAll();

private:
    mongocxx::gridfs::bucket bucket;
    mongocxx::collection bucketFiles;
    mongocxx::collection bucketChunks;
};

class MongoCollection : public NoSqlCollection {
public:
    explicit MongoCollection( mongocxx::collection _coll ) : collection( std::move( _coll )) {}

    mongocxx::collection operator()() const { return collection; }

    mongocxx::change_stream watch();

private:
    mongocxx::collection collection;
};

class MongoObjectId : public NoSqlId {
public:
    explicit MongoObjectId( bsoncxx::types::value _id ) : id( std::move( _id )) {}

    explicit MongoObjectId( strview _id ) : id( bsoncxx::types::b_oid{ bsoncxx::oid{ _id.data(), _id.length() }} ) {}

    bsoncxx::types::value operator()() const { return id; }

private:
    bsoncxx::types::value id;
};

class MongoDocumentValue : public NoSqlDocumentView {
public:
    explicit MongoDocumentValue( bsoncxx::document::value _doc ) : value( std::move( _doc )) {}

    [[nodiscard]] bsoncxx::document::view view() const { return value.view(); }

    bsoncxx::document::value operator()() const { return value; }

private:
    bsoncxx::document::value value;
};

//class MongoDocumentArray : public NoSqlDocumentArray {
//public:
//    explicit MongoDocumentArray( bsoncxx::document::a _doc ) : value( _doc ) {}
//
//    [[nodiscard]] bsoncxx::document::view view() const { return value.view(); }
//
//    bsoncxx::document::value operator()() const { return value; }
//
//private:
//    bsoncxx::document::value value;
//};

class MongoFileUpload {
public:
    explicit MongoFileUpload( mongocxx::result::gridfs::upload _value ) : value( std::move( _value )) {}

    mongocxx::result::gridfs::upload operator()() const { return value; }

    std::string getStringId() const {
        auto lId = value.id();
        auto lId8 = lId.get_oid();
        auto lId8v = lId8.value;
        return lId8v.to_string();
    }

private:
    mongocxx::result::gridfs::upload value;
};

struct StreamChangeMetadata {
    explicit StreamChangeMetadata( bsoncxx::document::view change ) : id( change["fullDocument"]["_id"].get_value()) {

        auto doc = change["fullDocument"];
        auto metadata = doc["metadata"];

        filename = doc["filename"].get_utf8().value;
        group = metadata["group"].get_utf8().value;
        username = metadata["username"].get_utf8().value;
        useremail = metadata["useremail"].get_utf8().value;
        project = metadata["project"].get_utf8().value;
        hash = metadata["hash"] ? metadata["hash"].get_utf8().value : "";
        contentType = metadata["contentType"] ? metadata["contentType"].get_utf8().value : "application/octet-stream";
        thumb = metadata["thumb"] ? metadata["thumb"].get_utf8().value : "";
        deps = metadata["deps"] ? metadata["deps"] : bsoncxx::document::element{};
    }

    MongoObjectId id;
    strview filename;
    strview group;
    strview username;
    strview useremail;
    strview project;
    strview contentType;
    strview hash;
    strview thumb;
    bsoncxx::document::element deps;
};

class Mongo : public NoSql {
public:
    Mongo() = default;

    explicit Mongo( const DBConnection &_cs );

    [[nodiscard]] MongoBucket useBucket( const std::string &_bucketName ) const;

    MongoCollection operator[]( const std::string &_collName ) const;

    static uint8_p
    fileDownload( MongoBucket &bucket, const MongoObjectId &_id, std::function<void( uint8_p && )> callback = nullptr );

    static std::string fileDownload( MongoBucket &bucket, const MongoObjectId &_id, const std::string &filename,
                                     std::function<void( const std::string & )> callback = nullptr );

    static std::optional<uint8_p> fileDownloadWithId( MongoBucket &bucket, const std::string &_id );

    static MongoFileUpload fileUpload( MongoBucket &bucket, const std::string &filename,
                                       uint8_p &&buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string & )> callback = nullptr );

    static MongoFileUpload fileUpload( MongoBucket &bucket, const std::string &filename,
                                       const SerializableContainer &buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string & )> callback = nullptr );

    static MongoDocumentValue
    FSMetadata( const std::string &group, strview project, strview uname,
                strview uemail,
                const std::string &contentType,
                const std::string &md5, const std::string &thumb, const ResourceDependencyDict &deps = {} );

    std::string insertEntityFromAsset( const StreamChangeMetadata &meta );

    template<typename N>
    std::string insertEntityFromAsset2( const N &meta ) {

        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::sub_array;

        auto builder = bsoncxx::builder::basic::document{};
        builder.append(
                kvp( "group", meta.group ),
                kvp( "source", meta.source ),
                kvp( "name", meta.name ),
                kvp( "project", meta.project ),
                kvp( "isPublic", meta.isPublic ),
                kvp( "isRestricted", meta.isRestricted ),
                kvp( "contentType", meta.contentType ),
                kvp( "hash", meta.hash ),
                kvp( "userId", meta.userId()),
                kvp( "thumb", meta.thumb ),
                kvp( "tags", [&]( sub_array sa ) {
                    for ( const auto &tag : split_tags( std::string( meta.name ))) {
                        sa.append( toLower( tag ));
                    }
                } )
        );

        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db["entities"].insert_one( builder.view());
        if ( result ) {
            auto dbc = db["entities"].find_one(
                    bsoncxx::builder::stream::document{} << "_id" << ( *result ).inserted_id()
                                                         << bsoncxx::builder::stream::finalize );
            return bsoncxx::to_json( dbc->view());
        }

        return "";
    }

    template<typename N>
    void updateUploads( const N &meta ) {

        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::sub_array;

        auto filter = bsoncxx::builder::basic::document{};
        filter.append(
                kvp( "filename", meta.source ),
                kvp( "group", meta.group )
        );
        std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t( p );
        db["uploads"].update_one( filter.view(), bsoncxx::builder::stream::document{} << "$set"
                                                                                      << bsoncxx::builder::stream::open_document
                                                                                      <<
                                                                                      "completed" << meta.source
                                                                                      << "completedat"
                                                                                      << std::ctime( &t )
                                                                                      << bsoncxx::builder::stream::close_document
                                                                                      << bsoncxx::builder::stream::finalize );
    }

    void insertDaemonCrashLog( const std::string &crash );

private:
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};
    mongocxx::client conn;
    mongocxx::database db;
};



