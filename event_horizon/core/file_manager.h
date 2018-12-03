#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "htypes_shared.hpp"
#include "http/webclient.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "callback_dependency.h"

struct FileInfo {
	std::string fullPath;
	std::string name;
	std::string nameWithoutExt;
	std::string extension;
	uint64_t	attributes;
	uint64_t    date_modified;
	uint64_t    size;
};

using FileCallback = std::function<void(const uint8_p&)>;
using FileStringCallback = std::function<void(const std::string&)>;

bool callbackSuccessfulStatus( DependencyStatus dp );

struct CallbackData {
    virtual void init() {}
	bool hasFinishedLoading() const {
		return ( callbackSuccessfulStatus(status) ||
				 status == DependencyStatus::LoadingFailed );
	}
	bool isComplete() const {
		return ( status == DependencyStatus::CallbackSuccessfullyExectuted ||
				 status == DependencyStatus::CallbackFailedOnExectution );
	}

    std::string asString() const {
        return std::string( reinterpret_cast<const char*>( data.first.get() ), data.second );
    }
    virtual bool needsReload() const = 0;

    DependencyStatus status = DependencyStatus::Loading;
	uint8_p data;
};

struct CallbackDataNoReload : public CallbackData {
    virtual ~CallbackDataNoReload() {}

    bool needsReload() const override { return false; };
};

struct CallbackDataMaker : public CallbackData {
    CallbackDataMaker( std::shared_ptr<ResourceBuilder> _builder, DependencyMaker& _md ) : builder( _builder ), md( _md ) {}

    virtual ~CallbackDataMaker() {}

    std::shared_ptr<ResourceBuilder> builder;
    DependencyMaker& md;
    virtual bool needsReload() const override {
        return builder->isReloading();
    };
};

struct CallbackDataObservable : public CallbackData {
    CallbackDataObservable( std::shared_ptr<ResourceBuilderObservable> _builder ) : builderObservable( _builder ) {}

    virtual ~CallbackDataObservable() {}

    std::shared_ptr<ResourceBuilderObservable> builderObservable;
    virtual bool needsReload() const override {
        return false;
    };
};

struct FileCallbackHandler {
    virtual bool executeCallback( const DependencyStatus _status ) = 0;

    bool isComplete() const {
        return cbData->isComplete();
    }
    bool hasCompletedSuccessfully() const {
        return cbData->status == DependencyStatus::CallbackSuccessfullyExectuted;
    }
    bool needsReload() const {
        return cbData->needsReload();
    }

    std::shared_ptr<CallbackData> cbData;
};

struct FileCallbackHandlerSimple : FileCallbackHandler {
    FileCallbackHandlerSimple(std::function<void(const uint8_p&)> _simpleCallback) {
        cbData = std::make_shared<CallbackDataNoReload>();
        simpleCallback = _simpleCallback;
    }

    virtual ~FileCallbackHandlerSimple() {

    }

    bool executeCallback( const DependencyStatus _status ) {
        if ( callbackSuccessfulStatus(_status) ) {
            simpleCallback( cbData->data );
            return true;
        }
        return false;
    };

    std::function<void(const uint8_p&)> simpleCallback;
};

struct FileCallbackHandlerMaker : public FileCallbackHandler {
    FileCallbackHandlerMaker( std::shared_ptr<ResourceBuilder> _builder, DependencyMaker& _md ) {
        cbData = std::make_shared<CallbackDataMaker>( _builder, _md );
    }

    virtual ~FileCallbackHandlerMaker() {

    }

    bool executeCallback( const DependencyStatus _status ) {
        return std::dynamic_pointer_cast<CallbackDataMaker>(cbData)->builder->make(
               std::dynamic_pointer_cast<CallbackDataMaker>(cbData)->md, std::move( cbData->data ), _status );
    };
};

struct FileCallbackHandlerObservable : public FileCallbackHandler {

    FileCallbackHandlerObservable( std::shared_ptr<ResourceBuilderObservable> _builder ) {
        cbData = std::make_shared<CallbackDataObservable>( _builder );
    }

    virtual ~FileCallbackHandlerObservable() {

    }

    bool executeCallback( const DependencyStatus _status ) {
        return std::dynamic_pointer_cast<CallbackDataObservable>(cbData)->builderObservable->make( std::move( cbData->data ),
                                                                                         _status );
    };
};

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj, DependencyMaker& _md ) {
    return std::make_shared<FileCallbackHandlerMaker>( std::make_shared<Builder>( obj ), _md );
}

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj ) {
    return std::make_shared<FileCallbackHandlerObservable>( std::make_shared<Builder>( obj ) );
}

namespace FileManager {

    bool useFileSystemCachePolicy();
	bool fileExist( const std::string& filename );
	bool fileExistAbs( const std::string& filename );
	uint64_t fileDate( const std::string& filename );
	std::string filenameOnly( const std::string& input );
	std::string filenameOnlyNoExtension( const std::string& input );

	// Data Exchange
	void setCallbackData( const Http::Result& header );

	// Remote file access
	void readRemoteFileInternal( const Url& uri );

    void readRemote( const Url& url, std::shared_ptr<FileCallbackHandler> _handler,
                     Http::ResponseFlags rf = Http::ResponseFlags::None );

    template<HttpQuery Q, typename T>
    Url makeUrl(const std::string& _name) {
        switch (Q) {
            case HttpQuery::JSON:
                return Url::privateAPI(HttpFilePrefix::catalog + T::typeName() + HttpFilePrefix::getname + url_encode( _name ) );
                break;
            case HttpQuery::Binary:
            case HttpQuery::Text: {
                if ( T::usesNotExactQuery() ) {
                    return Url( HttpFilePrefix::entities_onebinary + url_encode_spacesonly(_name) );
                } else {
                    return Url( HttpFilePrefix::get + url_encode( _name ) );
                }
            }
            break;
            default:
                break;
        }
        return Url{};
    }

    template<typename B, HttpQuery Q, typename T = B>
    void readRemote( const std::string& _name, B& _builder, DependencyMaker& _md ) {
        readRemote( makeUrl<Q, T>(_name), makeHandler<B>( _builder , _md ) );
    }

    template<typename B, HttpQuery Q, typename T = B>
    void readRemote( const std::string& _name, B& _builder ) {
        readRemote( makeUrl<Q, T>(_name), makeHandler<B>( _builder ) );
    }

    void readRemoteSimpleCallback( const std::string& filename, ResponseCallbackFunc simpleCallback );

    void writeRemoteFile( const std::string& filename, const char *buff, uint64_t length,
                          HttpUrlEncode _filenameEnc = HttpUrlEncode::Yes );
    void writeRemoteFile( const std::string& _filename, const std::vector<unsigned char>& _data );
//	void renameRemoteFile( const std::string& filename, const std::string& newfilename );
//	void removeRemoteFile( const std::string& filename );
//	std::vector<FileInfo> listRemoteFolder( const std::string& _folderName, const std::string& _maskOut = "" );
//	void makeRemoteDir( const std::string& dirname );

	// Local file access
    uint8_p readLocalFile( const std::string& filename );
	std::unique_ptr<uint8_t[]> readLocalFile( const std::string& filename, uint64_t& _length,
											  bool addTrailingZero = false );
	std::string readLocalTextFile( const std::string& filename );
	std::string readLocalTextFile( const std::wstring& filename );
	bool writeLocalFile( const std::string& filename, const char *buff, uint64_t length, bool isFullPath = false );
	bool writeLocalFile( const std::string& filename, const rapidjson::StringBuffer& s, bool isFullPath = false );
    bool writeLocalFile( const std::string& filename, const std::vector<unsigned char>& s );
    bool writeLocalTextFile( const std::string& filename, const std::string& s, bool isFullPath = false );
    void makeLocalDir( const std::string& dirName );
	void copyLocalFile( const std::string& src_path, const std::string& dst_path );
	void deleteLocalFile( const std::string& src_path );

	// Transfers
	void copyLocalToRemote( const std::string& source, const std::string& dest,
	                        HttpUrlEncode _filenameEnc = HttpUrlEncode::Yes );

    extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
    extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;
}

namespace FM = FileManager;
