//
//  file_manager.cpp
//

#include "file_manager.h"
#include <fstream>
#include <sys/stat.h>

#include "http/webclient.h"
#include "util.h"

namespace FileManager {

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

// ********************************************************************************************************************
// REMOTE FILE SYSTEM
// ********************************************************************************************************************

    void readRemoteSimpleCallback( const std::string& filename,
                                   ResponseCallbackFunc simpleCallback ) {
        Http::get( Url( HttpFilePrefix::get + url_encode(filename)), simpleCallback, nullptr,
                        Http::ResponseFlags::ExcludeFromCache );
    }

    void writeRemoteFile( const std::string& _filename, const char *buff, uint64_t length,
                          HttpUrlEncode _filenameEnc ) {
        auto fn = _filenameEnc == HttpUrlEncode::Yes ? url_encode( _filename ) : _filename;
        Http::post( Url{HttpFilePrefix::fileupload + fn}, buff, length );
    }

    void writeRemoteFile( const std::string& _filename, const std::vector<unsigned char>& _data ) {
        Http::post( Url{HttpFilePrefix::fileupload + url_encode(_filename)}, _data );
    }

// ********************************************************************************************************************
// LOCAL FILE SYSTEM
// ********************************************************************************************************************

    void readLocalFile( const std::string& filename, SerializableContainer& _ret ) {
        std::ifstream file( filename, std::ios::in | std::ios::binary | std::ios::ate );
        if ( file.is_open()) {
            auto length = static_cast<uint64_t>(file.tellg());
            _ret.resize( length );
            file.seekg( 0, std::ios::beg );
            file.read( reinterpret_cast<char *>(_ret.data()), length );
            file.close();
        }
    }

    SerializableContainer readLocalFileC( const std::string& filename ) {
        SerializableContainer _ret;
        std::ifstream file( filename, std::ios::in | std::ios::binary | std::ios::ate );
        if ( file.is_open()) {
            auto length = static_cast<uint64_t>(file.tellg());
            _ret.resize( length );
            file.seekg( 0, std::ios::beg );
            file.read( reinterpret_cast<char *>(_ret.data()), length );
            file.close();
        }
        return _ret;
    }

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
            _length = static_cast<uint64_t>(file.tellg());
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

        std::string content;
        if ( !ifs.is_open()) {
            LOGR("Error opening local file %s", filename.c_str());
            return content;
        }

        ifs.seekg( 0, std::ios::end );
        content.reserve( ifs.tellg());
        ifs.seekg( 0, std::ios::beg );

        content.assign(( std::istreambuf_iterator<char>( ifs )), std::istreambuf_iterator<char>());

        return content;
    }

    std::vector<std::string> readLocalTextFileLineByLine( const std::string& filename ) {
        std::ifstream ifs( filename );

        std::vector<std::string> ret{};
        if ( !ifs.is_open()) {
            LOGR("Error opening local file %s", filename.c_str());
            return ret;
        }

        std::string line;
        while (!ifs.eof()) {
            std::getline( ifs, line );
            ret.emplace_back( line );
        }

        return ret;
    }

    std::string readLocalTextFile( const std::wstring& filename ) {
        return readLocalTextFile( std::string( filename.begin(), filename.end()) );
    }

    void copyLocalFile( const std::string& src_path, const std::string& dst_path ) {
        std::ifstream inputFile = std::ifstream( src_path, std::ios::in | std::ios::binary );
        std::ofstream outputFile = std::ofstream( dst_path, std::ios::out | std::ios::binary );

        inputFile.seekg( 0, inputFile.end );
        size_t length = static_cast<size_t>(inputFile.tellg());
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
        std::ofstream ffile( isFullPath ? filename : ( "/" + filename ) );
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
            persistanceSync();
            return true;
        } else {
            LOGRS("Cannot open " << filename << " on writeLocalTextFile");
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

    bool writeLocalFile( const std::string& filename, const std::vector<std::string>& s ) {
        std::ofstream fp(filename);
        if ( fp.is_open()) {
            for ( const auto& si : s ) {
                fp << si << std::endl;
            }
            fp.close();
            return true;
        }
        return false;
    }

    bool writeLocalFile( const std::string& filename, const std::stringstream& ss ) {
        std::ofstream fp(filename);
        if ( fp.is_open()) {
            fp << ss.rdbuf();
            fp.close();
            return true;
        }
        return false;
    }

    bool writeLocalFile( const std::string& filename, const std::string& s ) {
        std::ofstream fp(filename);
        if ( fp.is_open()) {
            fp << s;
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
