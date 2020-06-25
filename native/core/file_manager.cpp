//
//  file_manager.cpp
//

#include "file_manager.h"
#include <fstream>
#include <sys/stat.h>
#include <filesystem>

#include "http/webclient.h"
#include "util.h"

size_t fcread(void *cookie, char *buf, size_t size);
size_t fclength(void *cookie);
size_t fcwrite(void *cookie, const char *buf, int size);
size_t fcseek(void *cookie, fpos_t offset, int whence);
void fcclose(void *cookie);
FILE * fcopen(const char *filename, const char *mode);

bool isFilenameAFolder( const std::string& input ) {
    size_t pos = input.find_last_of( "/" );
    if ( pos != std::string::npos && pos == input.size() - 1 ) return true;
    pos = input.find_last_of( "\\" );
    if ( pos != std::string::npos && pos == input.size() - 1 ) return true;

    return false;
}

std::string getFileNameNoExt( const std::string& input ) {
    std::string ret = input;
    size_t pos = ret.find_last_of( "." );
    if ( pos != std::string::npos ) {
        ret = ret.substr( 0, pos );
    }

    return ret;
}

std::string getFileNameKey( const std::string& input, EncodingStatusFlag ef ) {
    return getFileNameNoExt( getFileName( input, ef ) );
}


std::string getFileNameCallbackKey( const std::string& input ) {
    return url_decode(getFileNameKey( input, EncodingStatusFlag::NotEncode));
}

std::string getFileNameExt( const std::string& input, bool noDot ) {
    std::string ret = "";
    size_t pos = input.find_last_of( "." ) + (noDot ? 1 : 0);
    if ( pos != std::string::npos ) {
        ret = input.substr( pos, input.length() - pos );
    }

    return ret;
}

std::string getFileNameExtToLower( const std::string& input ) {
    return toLower(getFileNameExt(input));
}

std::string getFileName( const std::string& input, EncodingStatusFlag ef ) {
    std::string ret = input;
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( pos + 1, ret.length() );
    }
    pos = ret.find_last_of( "\\" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( pos + 1, ret.length() );
    }

    if ( checkBitWiseFlag(ef, EncodingStatusFlag::DoEncode ) ) {
        pos = ret.rfind( "%2F" );
        if ( pos != std::string::npos ) {
            ret = ret.substr( pos + 3, ret.length() );
        }
    }

    return ret;
}

std::string getFileNamePath( const std::string& input ) {
    std::string ret = input;
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        return ret.substr( 0, pos );
    }
    pos = ret.find_last_of( "\\" );
    if ( pos != std::string::npos ) {
        return ret.substr( 0, pos );
    }

    return ret;
}

std::string getFileNameOnly( const std::string& input ) {
    std::string ret = input;
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( pos + 1, ret.length() );
    }
    pos = ret.find_last_of( "\\" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( pos + 1, ret.length() );
    }
    pos = ret.find_last_of( "." );
    if ( pos != std::string::npos ) {
        ret = ret.substr( 0, pos );
    }

    return ret;
}

std::string getLastEntryInPath( const std::string& input ) {
    std::string ret = input;
    if ( ret[ret.size()-1] == '/' ) {
        ret = ret.substr(0, ret.size()-1);
    }
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( pos + 1, ret.length() );
    }

    return ret;
}

std::string getLastFolderInPath( const std::string& input ) {
    std::string ret = input;
    if ( ret[ret.size()-1] == '/' ) {
        ret = ret.substr(0, ret.size()-1);
    }
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        ret = ret.substr( 0, pos );
        pos = ret.find_last_of( "/" );
        if ( pos != std::string::npos ) {
            ret = ret.substr( pos + 1, ret.length() );
        }
    }

    return ret;
}

std::string getFirstFolderInPath( const std::string& input ) {
    std::string ret = input;
    if ( ret[ret.size()-1] == '/' ) {
        ret = ret.substr(0, ret.size()-1);
    }
    // trim path before filename
    size_t pos = ret.find_first_of( "/" );
    if ( pos != std::string::npos ) {
        size_t pos2 = ret.find_first_of( "/", pos );
        if ( pos2 != std::string::npos ) {
            return input.substr( pos, pos2 - pos );
        }
    }

    return ret;
}

bool isFileExtAnImage( const std::string& _ext ) {

    auto ext = toLower(_ext);

    if ( ext == ".jpg" ) return true;
    if ( ext == ".png" ) return true;
    if ( ext == ".jpeg" ) return true;
    if ( ext == ".tga" ) return true;
    if ( ext == ".bmp" ) return true;
    if ( ext == ".psd" ) return true;
    if ( ext == ".gif" ) return true;
    if ( ext == ".hdr" ) return true;
    if ( ext == ".pic" ) return true;
    if ( ext == ".exr" ) return true;

    return false;
}

bool nameHasImageExtension( const std::string& input ) {
    std::string ext = getFileNameExtToLower(input);
    return ext != "" && ( ext == ".jpg" || ext == ".jpeg" || ext == ".png" );
}

bool isFileExtCompressedArchive( const std::string& _filename ) {
    return getFileNameExt( std::string( _filename )) == ".zip";
}

namespace FileManager {

//    std::string filenameOnlyNoExtension( const std::string& input ) {
//        std::string ret = filenameOnly( input );
//        size_t pos = ret.find_last_of( "." );
//        if ( pos != std::string::npos ) {
//            ret = ret.substr( 0, pos );
//        }
//        return ret;
//    }

    bool fileExist( const std::string& filename ) {
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

    void writeRemoteEntity( const std::string& _filename, const std::string& _group, const SerializableContainer& data ) {
        auto fn = url_encode( _filename );
        Http::post(
    Url{HttpFilePrefix::entities + _group + "/" + _filename}, data );
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
        if ( Url::isValidUrl(filename) ) {
            return Http::getSync( filename );
        }
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

        std::string fullPath = systemRootDir() + filename;
        FILE *afile = fcopen(fullPath.c_str(), "rb");
        if (afile) {
            _length = (uint64_t)fclength(afile);
            memblock = std::make_unique<uint8_t[]>( _length + addTrailingZero );
            fcread( afile, reinterpret_cast<char *>( memblock.get()), _length);
            if ( addTrailingZero ) {
                memblock[_length] = 0;
            }
            fcclose(afile);
        }
        return memblock;
    }

    std::string readLocalTextFile( const std::string& filename ) {
        std::string fullPath = systemRootDir() + filename;
        std::string content;

        FILE *afile = fcopen(fullPath.c_str(), "rt");
        if (afile) {
            auto _length = (uint64_t)fclength(afile);
            content.resize( _length );
            fcread( afile, content.data(), _length);
//            content[_length] = '\0';
            fcclose(afile);
        }

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
        std::filesystem::create_directories( std::filesystem::path( getFileNamePath(filename) ));
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
