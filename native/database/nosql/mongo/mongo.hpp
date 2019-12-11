//
// Created by Dado on 09/12/2019.
//

#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/gridfs/downloader.hpp>

#include <core/util.h>

#include "../no_sql.hpp"

using strview = boost::string_view;

class MongoBucket : public NoSqlStorage {
public:
    explicit MongoBucket( mongocxx::gridfs::bucket entityBucket ) : bucket( std::move( entityBucket )) {}

    mongocxx::gridfs::bucket operator()() { return bucket; }

private:
    mongocxx::gridfs::bucket bucket;
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

    explicit MongoObjectId( strview _id ) : id( bsoncxx::types::b_oid{ bsoncxx::oid{ _id }} ) {}

    bsoncxx::types::value operator()() const { return id; }

private:
    bsoncxx::types::value id;
};

class MongoDocumentValue : public NoSqlDocumentView {
public:
    explicit MongoDocumentValue( bsoncxx::document::value _doc ) : value( _doc ) {}

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
        md5 = metadata["md5"] ? metadata["md5"].get_utf8().value : "";
        thumb = metadata["thumb"] ? metadata["thumb"].get_utf8().value : "";
        deps = metadata["deps"] ? metadata["deps"] : bsoncxx::document::element{};
    }

    MongoObjectId id;
    strview filename;
    strview group;
    strview username;
    strview useremail;
    strview project;
    strview md5;
    strview thumb;
    bsoncxx::document::element deps;
};

class Mongo : public NoSql {
public:
    Mongo() = default;
    explicit Mongo( const DBConnection& _cs );

    [[nodiscard]] MongoBucket useBucket( const std::string& _bucketName ) const;
    MongoCollection operator[]( const std::string& _collName ) const;

    static void
    fileDownload( MongoBucket& bucket, const MongoObjectId& _id, std::function<void( uint8_p&& )> callback = nullptr );
    static void fileDownload( MongoBucket& bucket, const MongoObjectId& _id, const std::string& filename,
                              std::function<void( const std::string& )> callback = nullptr );
    static std::optional<uint8_p> fileDownloadWithId( MongoBucket& bucket, const std::string& _id );
    static MongoFileUpload fileUpload( MongoBucket& bucket, const std::string& filename,
                                       uint8_p&& buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string& )> callback = nullptr );
    static MongoFileUpload fileUpload( MongoBucket& bucket, const std::string& filename,
                                       const SerializableContainer& buffer, MongoDocumentValue doc,
                                       std::function<void( const std::string& )> callback = nullptr );

    static MongoDocumentValue
    FSMetadata( const std::string& group, strview project, strview uname,
                strview uemail,
                const std::string& md5, const std::string& thumb, const ResourceDependencyDict& deps = {} );

    void insertEntityFromAsset( const StreamChangeMetadata& meta );
private:
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};
    mongocxx::client conn;
    mongocxx::database db;
};



