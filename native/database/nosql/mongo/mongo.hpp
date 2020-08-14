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

    [[maybe_unused]] void deleteAll();

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

    [[maybe_unused]] [[nodiscard]] std::string getStringId() const {
        auto lId = value.id();
        auto lId8 = lId.get_oid();
        auto lId8v = lId8.value;
        return lId8v.to_string();
    }

private:
    mongocxx::result::gridfs::upload value;
};

struct [[maybe_unused]] StreamChangeMetadata {
    [[maybe_unused]] explicit StreamChangeMetadata( bsoncxx::document::view change ) : id( change["fullDocument"]["_id"].get_value()) {

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

    [[maybe_unused]] [[nodiscard]] MongoBucket useBucket( const std::string &_bucketName ) const;

    MongoCollection operator[]( const std::string &_collName ) const;

    [[maybe_unused]] static uint8_p
    fileDownload( MongoBucket &bucket, const MongoObjectId &_id, std::function<void( uint8_p && )> callback = nullptr );

    [[maybe_unused]] static std::string fileDownload( MongoBucket &bucket, const MongoObjectId &_id, const std::string &filename,
                                     std::function<void( const std::string & )> callback = nullptr );

    [[maybe_unused]] static std::optional<uint8_p> fileDownloadWithId( MongoBucket &bucket, const std::string &_id );

    static MongoFileUpload fileUpload( MongoBucket &bucket, const std::string &filename,
                                       uint8_p &&buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string & )> callback = nullptr );

    static MongoFileUpload fileUpload( MongoBucket &bucket, const std::string &filename,
                                       const SerializableContainer &buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string & )> callback = nullptr );

    [[maybe_unused]] static MongoDocumentValue
    FSMetadata( const std::string &group, strview project, strview uname,
                strview uEmail,
                const std::string &contentType,
                const std::string &md5, const std::string &thumb, const ResourceDependencyDict &deps = {} );

    [[maybe_unused]] std::string insertEntityFromAsset( const StreamChangeMetadata &meta );

    template<typename N>
    MongoObjectId upsertEntity( N &meta ) {

        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::sub_array;
        using bsoncxx::builder::basic::sub_document;

        auto builder = bsoncxx::builder::basic::document{};
        builder.append( kvp( "$set",
                             [&]( sub_document subDoc ) {
                                 subDoc.append(
                                         kvp( "group", meta.group ),
                                         kvp( "source", meta.source ),
                                         kvp( "name", meta.name ),
                                         kvp( "project", meta.project ),
                                         kvp( "isPublic", meta.isPublic ),
                                         kvp( "isRestricted", meta.isRestricted ),
                                         kvp( "contentType", meta.contentType ),
                                         kvp( "hash", meta.hash ),
                                         kvp( "userId", meta.userId()),
                                         kvp( "bbox3d", [&]( sub_array sa ) {
                                             sa.append( meta.bbox3d.minPoint()[0] );
                                             sa.append( meta.bbox3d.minPoint()[1] );
                                             sa.append( meta.bbox3d.minPoint()[2] );
                                             sa.append( meta.bbox3d.maxPoint()[0] );
                                             sa.append( meta.bbox3d.maxPoint()[1] );
                                             sa.append( meta.bbox3d.maxPoint()[2] );
                                         } ),
                                         kvp( "bboxSize", [&]( sub_array sa ) {
                                             sa.append( meta.bboxSize[0] );
                                             sa.append( meta.bboxSize[1] );
                                             sa.append( meta.bboxSize[2] );
                                         } ),
                                         kvp( "tags", [&]( sub_array sa ) {
                                             for ( const auto &tag : split_tags( std::string( meta.name ))) {
                                                 sa.append( toLower( tag ));
                                             }
                                         } ));
                             } ));

        auto filter = bsoncxx::builder::basic::document{};
        filter.append(
                kvp( "filename", meta.source ),
                kvp( "group", meta.group )
        );

//        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = db["entities"].insert_one( builder.view());
        mongocxx::options::update options;
        options.upsert( true );
        auto ret = db["entities"].update_one( filter.view(), builder.view(), options );
        auto oid = ret->upserted_id();
        if ( !oid ) {
            auto retDocument = db["entities"].find_one( filter.view() );
                return MongoObjectId{(retDocument->view())["_id"].get_value()};
        }
        return MongoObjectId{oid->get_value()};
    }

    template<typename N, typename M>
    void upsertBBox( M doc, const N &bbox3d ) {

        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::sub_array;
        using bsoncxx::builder::basic::sub_document;

        auto builder = bsoncxx::builder::basic::document{};
        builder.append( kvp( "$set",
                             [&]( sub_document subDoc ) {
                                 subDoc.append(
                                         kvp( "bbox3d", [&]( sub_array sa ) {
                                             sa.append( bbox3d.minPoint()[0] );
                                             sa.append( bbox3d.minPoint()[1] );
                                             sa.append( bbox3d.minPoint()[2] );
                                             sa.append( bbox3d.maxPoint()[0] );
                                             sa.append( bbox3d.maxPoint()[1] );
                                             sa.append( bbox3d.maxPoint()[2] );
                                         } ),
                                         kvp( "bboxSize", [&]( sub_array sa ) {
                                             sa.append( bbox3d.size()[0] );
                                             sa.append( bbox3d.size()[1] );
                                             sa.append( bbox3d.size()[2] );
                                         } ));
                             } ));

        auto filter = bsoncxx::builder::basic::document{};
        filter.append( kvp( "_id", doc["_id"].get_oid().value ));
        db["entities"].update_one( filter.view(), builder.view());
    }

    template<typename N>
    void updateUploads( const N &meta, const MongoObjectId& id ) {

        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::sub_array;

        auto filter = bsoncxx::builder::basic::document{};
        filter.append(
                kvp( "filename", meta.source ),
                kvp( "group", meta.group ),
                kvp( "project", meta.project ),
                kvp( "userId", meta.userId() ),
                kvp( "entityId", id() )
        );
        db["completed_uploads"].insert_one( filter.view() );
    }

    mongocxx::cursor find( const std::string &collection, const std::vector<std::string> &query ) {

        auto filter = bsoncxx::builder::basic::document{};
        using bsoncxx::builder::basic::kvp;
        filter.append(
                kvp( query[0], query[1] )
        );

        std::vector<std::string> ret{};
        return db["entities"].find( filter.view());
    }

    void insertDaemonCrashLog( const std::string &crash, const std::string &username );

private:
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};
    mongocxx::client conn;
    mongocxx::database db;
};



