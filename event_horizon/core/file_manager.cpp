//
//  file_manager.cpp
//

#include "file_manager.h"
#include <fstream>
#include <sys/stat.h>

#include "http/webclient.h"
#include "util.h"

bool callbackSuccessfulStatus( DependencyStatus dp ) {
    if ( dp == DependencyStatus::LoadedSuccessfully ||
         dp == DependencyStatus::LoadedSuccessfully204 ) {
        return true;
    }
    return false;
}

namespace FileManager {

    std::unordered_map< std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
    std::unordered_map< std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;

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

        if ( fileExistAbs( cachedFileName ) ) {
            std::string bufferString;
            std::unique_ptr<uint8_t[]> bufferData;
            uint64_t bufferLength = 0;
            if ( checkBitWiseFlag(header.flags, Http::ResponseFlags::Text) ) {
                bufferString = readLocalTextFile(cachedFileName);
            } else {
                bufferData = readLocalFile( cachedFileName, bufferLength );
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
            Http::get( cachedUrl.uri, cacheSave );
        }
    }

    std::string filenameOnly( const std::string& input ) {
        std::string ret = input;
        // trim path before filename
        size_t pos = ret.find_last_of( "/" );
        if ( pos != std::string::npos ) {
            ret = ret.substr( pos + 1, ret.length());
        }
        pos = ret.find_last_of( "\\" );
        if ( pos != std::string::npos ) {
            ret = ret.substr( pos + 1, ret.length());
        }

        return ret;
    }

    std::string filenameOnlyNoExtension( const std::string& input ) {
        std::string ret = filenameOnly( input );
        size_t pos = ret.find_last_of( "." );
        if ( pos != std::string::npos ) {
            ret = ret.substr( 0, pos );
        }
        return ret;
    }

    bool fileExist( const std::string& filename ) {
        std::ofstream ffile( "data/" + filename, std::ios::in | std::ios::binary );
        if ( ffile.is_open()) {
            ffile.close();
            return true;
        }
        std::ofstream ffileAbsolute( filename, std::ios::in | std::ios::binary );
        if ( ffileAbsolute.is_open()) {
            ffileAbsolute.close();
            return true;
        }

        return false;
    }

    bool fileExistAbs( const std::string& filename ) {
        std::ofstream ffileAbsolute( filename, std::ios::in | std::ios::binary );
        if ( ffileAbsolute.is_open()) {
            ffileAbsolute.close();
            return true;
        }

        return false;
    }

    uint64_t fileDate( const std::string& filename ) {
        struct stat result;
        std::string completeFileName = ( "data/" + filename );

        if ( stat( completeFileName.c_str(), &result ) == 0 ) {
            auto mod_time = result.st_mtime;
            return mod_time;
        }
        return 0;
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

// ********************************************************************************************************************
// REMOTE FILE SYSTEM
// ********************************************************************************************************************

    void readRemoteSimpleCallback( const std::string& filename,
                                   std::function<void(const Http::Result&)> simpleCallback ) {
        Http::get( Url( HttpFilePrefix::get + url_encode(filename)), simpleCallback,
                        Http::ResponseFlags::ExcludeFromCache );
    }

    void readRemote( const Url& url, std::shared_ptr<FileCallbackHandler> _handler, Http::ResponseFlags rf ) {
        auto fkey = getFileNameCallbackKey( url.toString() );
        if ( _handler->needsReload() || callbacksDataMap.find( fkey ) == callbacksDataMap.end() ) {
            callbacksDataMap.insert( { fkey, _handler } );
            bool useFS = useFileSystemCachePolicy()
                         && url.isEngine( HttpFilePrefix::get )
                         && (!_handler->needsReload());
            auto rfe = useFS ? Http::ResponseFlags::HeaderOnly : rf;
            if (_handler->needsReload()) orBitWiseFlag( rfe, Http::ResponseFlags::ExcludeFromCache);

            Http::get( url, useFS ? cacheLoad : setCallbackData, rfe );
        }
    }

    void writeRemoteFile( const std::string& _filename, const char *buff, uint64_t length,
                          HttpUrlEncode _filenameEnc ) {
        auto fn = _filenameEnc == HttpUrlEncode::Yes ? url_encode( _filename ) : _filename;
        Http::postInternal( Url{HttpFilePrefix::fileupload + fn}, buff, length );
    }

    void writeRemoteFile( const std::string& _filename, const std::vector<unsigned char>& _data ) {
        Http::post( Url{HttpFilePrefix::fileupload + url_encode(_filename)}, _data );
    }

// ********************************************************************************************************************
// LOCAL FILE SYSTEM
// ********************************************************************************************************************

    uint8_p readLocalFile( const std::string& filename ) {
        uint8_p ret;
        ret.second = 0;
        ret.first = readLocalFile( filename, ret.second );
        return ret;
    }

    std::unique_ptr<uint8_t[]> readLocalFile( const std::string& filename, uint64_t& _length,
                                              bool addTrailingZero ) {
        std::unique_ptr<uint8_t[]> memblock;

        std::ifstream file( filename, std::ios::in | std::ios::binary | std::ios::ate );
        if ( file.is_open()) {
            _length = file.tellg();
            memblock = std::make_unique<uint8_t[]>( _length + addTrailingZero );
            file.seekg( 0, std::ios::beg );
            file.read( reinterpret_cast<char *>( memblock.get()), _length );
            file.close();
        }

        if ( addTrailingZero ) {
            memblock[_length] = 0;
        }
        return memblock;
    }

    std::string readLocalTextFile( const std::string& filename ) {
        std::ifstream ifs( filename );

        if ( !ifs.is_open()) throw "Error opening local file " + filename;

        std::string content;

        ifs.seekg( 0, std::ios::end );
        content.reserve( ifs.tellg());
        ifs.seekg( 0, std::ios::beg );

        content.assign(( std::istreambuf_iterator<char>( ifs )), std::istreambuf_iterator<char>());

        return content;
    }

    std::string readLocalTextFile( const std::wstring& filename ) {
        return readLocalTextFile( std::string( filename.begin(), filename.end()) );
    }

    void copyLocalFile( const std::string& src_path, const std::string& dst_path ) {
        std::ifstream inputFile = std::ifstream( src_path, std::ios::in | std::ios::binary );
        std::ofstream outputFile = std::ofstream( dst_path, std::ios::out | std::ios::binary );

        inputFile.seekg( 0, inputFile.end );
        size_t length = inputFile.tellg();
        inputFile.seekg( 0, inputFile.beg );

        // read the entire file table block into memory
        char *fileBuffer = new char[length];
        inputFile.read( fileBuffer, length );
        outputFile.write( fileBuffer, length );

        inputFile.close();
        outputFile.close();
    }

    void deleteLocalFile( const std::string& src_path ) {
        std::remove( src_path.c_str());
    }

    bool
    writeLocalFile( const std::string& filename, const char *buff, uint64_t length, bool isFullPath ) {
        std::ofstream ffile( isFullPath ? filename : ( "/" + filename ), std::ios::out | std::ios::binary );
        if ( ffile.is_open()) {
            ffile.write( buff, length );
            ffile.close();
            return true;
        }
        return false;
    }

    bool writeLocalTextFile( const std::string& filename, const std::string& s, bool isFullPath ) {
        std::ofstream fp( isFullPath ? filename : ( filename ));
        if ( fp.is_open()) {
            fp << s;
            fp.close();
            return true;
        }
        return false;
    }

    bool writeLocalFile( const std::string& filename, const rapidjson::StringBuffer& s, bool isFullPath ) {
        std::ofstream fp( isFullPath ? filename : ( "/" + filename ));
        if ( fp.is_open()) {
            fp << s.GetString();
            fp.close();
            return true;
        }
        return false;
    }

    bool writeLocalFile( const std::string& filename, const std::vector<unsigned char>& s ) {
        std::ofstream fp(filename);
        if ( fp.is_open()) {
            for ( const auto& b : s ) fp << b;
            fp.close();
            return true;
        }
        return false;
    }

// ********************************************************************************************************************
// TRANSFERS
// ********************************************************************************************************************

    void copyLocalToRemote( const std::string& source, const std::string& dest, HttpUrlEncode _filenameEnc ) {
        uint64_t l = 0;
        auto b = readLocalFile( source, l );
        if ( !b ) {
            LOGR("Impossible to open local file: %s", source.c_str());
        } else {
            writeRemoteFile( dest, reinterpret_cast<const char *>(b.get()), l, _filenameEnc );
        }
    }

}
