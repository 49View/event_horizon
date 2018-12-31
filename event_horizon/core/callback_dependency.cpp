//
// Created by Dado on 11/01/2018.
//

#include "callback_dependency.h"
#include "core/util.h"
#include "core/file_manager.h"
#include "core/builders.hpp"

dep_map g_deps;
std::unordered_map< std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
std::unordered_map< std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;

bool callbackSuccessfulStatus( DependencyStatus dp ) {
    if ( dp == DependencyStatus::LoadedSuccessfully ||
         dp == DependencyStatus::LoadedSuccessfully204 ) {
        return true;
    }
    return false;
}

namespace DependencyHandler {

    dep_map& dependencies() {
        return g_deps;
    };

    bool needsDependencies( [[maybe_unused]] const std::string& _key ) {
        return true;
    }

    void update() {
        // Two steps here;

        // 1st this is the FileManager callback queue
        // it's quite concrete and just deals with loading data in
        for ( auto& q : callbacksDataMap ) {
            auto qd = q.second->cbData;
            // Check if our callbacks have returned from loading (either successfully or not)
            // (IsComplete just checks for callback has reached the end of it's life, we do not know at this point
            // if the file has been loaded correctly or failed)
            if ( !q.second->isComplete()) {
                // Now if the file has been loaded correctly then execute the file callback related to that file
                if ( qd->hasFinishedLoading() ) {
                    bool cbStatus = q.second->executeCallback(qd->status);
                    // Also tag the status as "it has passed the loading stage, was it successful or not? in the
                    // callback ?
                    qd->status = cbStatus ? DependencyStatus::CallbackSuccessfullyExectuted
                                          : DependencyStatus::CallbackFailedOnExectution;
                    callbacksDataMapExecuted[q.first] = q.second;
                }
                // If it has failed to laod then just leave the status as it is (LoadingFailed)
            }
        }
        for ( auto it = callbacksDataMap.begin(); it != callbacksDataMap.end();) {
            if ( ( *it ).second->isComplete() ) { it = callbacksDataMap.erase( it ); } else { ++it; }
        }

        // 2nd This is a list of all dependencies file loads might have
        // An entry contains a list of dependencies key (string at the moment)
        for ( auto& v : DH::dependencies()) {
            // This complete return true only if every loading from the dependency list has been completed either
            // successfull or not, and once it has it will call the callback for the dependency. At that point the
            // entry will be tag as completed and won't enter this if case again
            if ( !v->isComplete()) {
                bool isComplete = true; // just means it has passed all the loading stage
                bool hasCompletedSuccessfully = true; // it means it has passed all the loading stages and callbacks
                // correctly
                for ( const auto& d : v->deps() ) {
                    auto it = callbacksDataMapExecuted.find( d );
                    if ( it == callbacksDataMapExecuted.end()) {
                        isComplete = false;
                        break;
                    } else {
                        hasCompletedSuccessfully &= it->second->hasCompletedSuccessfully();
                    }
                }
                if ( isComplete ) {
                    // this will execute even if there have been errors loading any resource because we can handle it
                    // inside the callback, so it's the callback job to decide how to handle the failures, but
                    // default the base class will just not execute the callback internally
                    v->execute( hasCompletedSuccessfully );
                    // Ok we break the cycle because execute can invalidate the iterator hence crashing
                    // TODO: maybe a restart instead of a "break and continue on next frame" ?
                    break;
                }
            }
        }

        DH::dependencies().erase( remove_if( DH::dependencies().begin(), DH::dependencies().end(), []( const auto& v)
                -> bool { return v->isComplete(); } ), DH::dependencies().end() );
    }

};

std::set<std::string> BaseBuilder::generateTags() const {
    std::set<std::string> ret = tags;
    auto lcname = toLower( name );
    auto ltags = split( lcname, "_" );
    for ( const auto& v : ltags ) {
        ret.emplace( v );
    }
    return ret;
}

bool CallbackDataMaker::needsReload() const {
    return builder->isReloading();
}

FileCallbackHandlerMaker::FileCallbackHandlerMaker( std::shared_ptr<ResourceBuilder> _builder, DependencyMaker& _md ) {
    cbData = std::make_shared<CallbackDataMaker>( _builder, _md );
}

bool FileCallbackHandlerMaker::executeCallback( const DependencyStatus _status ) {
    return std::dynamic_pointer_cast<CallbackDataMaker>(cbData)->builder->make(
            std::dynamic_pointer_cast<CallbackDataMaker>(cbData)->md, std::move( cbData->data ), _status );
}

FileCallbackHandlerObservable::FileCallbackHandlerObservable( std::shared_ptr<ResourceBuilderObservable> _builder ) {
    cbData = std::make_shared<CallbackDataObservable>( _builder );
}

bool FileCallbackHandlerObservable::executeCallback( const DependencyStatus _status ) {
    return std::dynamic_pointer_cast<CallbackDataObservable>(cbData)->builderObservable->make( std::move( cbData->data ),
                                                                                               _status );
}

void setCallbackData( const Http::Result& header ) {

    std::string key = getFileNameCallbackKey( header.uri );
    auto it = callbacksDataMap.find( key );
    auto& v = it->second->cbData;

    if ( !header.isSuccessStatusCode() ) {
        LOGE( "[ERROR] %s, %d, %s", key.c_str(), header.statusCode, header.uri.c_str() );
        v->status = DependencyStatus::LoadingFailed;
        return;
    }

    LOGR( "[LOAD OK] %s - %llu bytes", key.c_str(), header.length );
    v->status = header.length == 0 ? DependencyStatus::LoadedSuccessfully204 : DependencyStatus::LoadedSuccessfully;
    v->data.second = header.length;
    if ( v->status == DependencyStatus::LoadedSuccessfully ) {
        if ( header.flags == Http::ResponseFlags::Text  ) {
            std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(header.length);
            std::memcpy( buf.get(), header.bufferString.c_str(), header.length );
            v->data = { std::move( buf ), header.length };
        } else {
            v->data = { std::move( header.buffer ), header.length };
        }
    }
}

// Caching
// *******

// File cache (desktop mainly, web has its own cache)
std::string getTimeStampCacheFileName( const std::string& cachedFileName, const std::string& etag ) {
    return cacheFolder() + cachedFileName + ".cachetime" + etag;
}

std::string getCacheFileName( const std::string& sourceFileName ) {
    return std::to_string( std::hash<std::string>{}( sourceFileName ));
}

void saveCacheFiles( const Http::Result& header ) {
    auto filename = getFileName(header.uri);
    bool statusOK = header.isSuccessStatusCode();
    if ( statusOK ) {
        LOGR( "[CLOUD LOAD] %s ", filename.c_str());
        // Save to local cache
        FM::writeLocalFile( cacheFolder() + url_encode( header.ETag ),
                            header.buffer.get() == nullptr ?
                            reinterpret_cast<const char *>( header.bufferString.c_str() ) :
                            reinterpret_cast<const char *>( header.buffer.get() ),
                            static_cast<uint64_t>( header.length ), true );
    }
}

bool checkInCache( const Http::Result& header ) {
    auto filename = getFileName(header.uri);
    // Calculate the hash of the filename in order to use it as a key for cache, we use an hash rather than the real name to avoid
    // storing folder structures into cache, which will be a flat list
    std::string cachedFileName = cacheFolder() + url_encode( header.ETag );

    if ( FM::fileExistAbs( cachedFileName ) ) {
        std::string bufferString;
        std::unique_ptr<uint8_t[]> bufferData;
        uint64_t bufferLength = 0;
        if ( checkBitWiseFlag(header.flags, Http::ResponseFlags::Text) ) {
            bufferString = FM::readLocalTextFile(cachedFileName);
        } else {
            bufferData = FM::readLocalFile( cachedFileName, bufferLength );
        }
        if ( bufferLength == 0 && bufferString.empty() ) {
            LOGR( "[CACHE CORRUPTED] %s", filename.c_str());
        } else {
            setCallbackData( {header.uri, std::move(bufferData), bufferLength, 200} );
            LOGR( "[CACHE LOAD] %s ", filename.c_str());
            return true;
        }
    }
    return false;
}

void cacheSave( const Http::Result& header ) {
    saveCacheFiles( header );
    setCallbackData( header );
}

void cacheLoad( const Http::Result& header ) {
#ifdef _FAST_FILE_LOAD_NO_TIMESTAMP_CHECK
    if ( UseCacheDirect()) {
            auto cachedFile = readLocalFile( cacheFolder + cachedFileName + fext, _length, addTrailingZero );
            if ( _length > 0 ) {
                LOGR( "[DIRECT CACHE LOAD] %s", filename.c_str());
                return cachedFile;
            }
        }
#endif
    if ( !checkInCache( header ) ) {
//            auto filename = getFileName( header.uri, EncodingStatusFlag::NotEncode );
//            Http::get( Url( HttpFilePrefix::get + filename), cacheSave );
        Url cachedUrl;
        cachedUrl.fromString( header.uri );
        Http::get( Url{cachedUrl.uri}, cacheSave );
    }
}

void readRemote( const Url& url, std::shared_ptr<FileCallbackHandler> _handler, Http::ResponseFlags rf ) {
    auto fkey = getFileNameCallbackKey( url.toString() );
    if ( _handler->needsReload() || callbacksDataMap.find( fkey ) == callbacksDataMap.end() ) {
        callbacksDataMap.insert( { fkey, _handler } );
        bool useFS = FM::useFileSystemCachePolicy()
                     && url.isEngine( HttpFilePrefix::get )
                     && (!_handler->needsReload());
        auto rfe = useFS ? Http::ResponseFlags::HeaderOnly : rf;
        if (_handler->needsReload()) orBitWiseFlag( rfe, Http::ResponseFlags::ExcludeFromCache);

        Http::get( url, useFS ? cacheLoad : setCallbackData, rfe );
    }
}
