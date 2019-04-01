/**
 * Read TAR file in C++
 * Example code
 *
 * (C) Uli Köhler 2013
 * Licensed under CC-By 3.0 Germany: http://creativecommons.org/licenses/by/3.0/de/legalcode
 *
 * Port to SixthView Davide Pirola 2018
 *
 * Compile like this:
 *   g++ -o cpptar cpptar.cpp -lboost_iostreams -lz -lbz2
 */

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <unistd.h>

#include "tar_util.h"
#include <core/file_manager.h>
#include <core/hashable.hpp>

namespace tarUtil {

    static const int TAR_HEADER_SIZE = 512;

    bool isTar( const SerializableContainer& fin ) {

        if ( fin.size() < TAR_HEADER_SIZE ) return false;

        char zeroBlock[512];
        memset( zeroBlock, 0, 512 );
        const unsigned char *rawBytes = fin.data();
        TARFileHeader currentFileHeader{};
        std::memcpy((char *) &currentFileHeader, rawBytes, TAR_HEADER_SIZE );

        return currentFileHeader.checkChecksum();
    }

    SerializableContainerDict untar( const SerializableContainer& fin ) {
        //Initialize a zero-filled block we can compare against (zero-filled header block --> end of TAR archive)
        SerializableContainerDict ret;
        char zeroBlock[512];
        memset( zeroBlock, 0, 512 );
        //Start reading
        bool nextEntryHasLongName = false;
        //size_t bytesRead = 0;
        const unsigned char *rawBytes = fin.data();

        while ( true ) { //Stop if end of file has been reached or any error occured
            TARFileHeader currentFileHeader{};
            //Read the file header.
            std::memcpy((char *) &currentFileHeader, rawBytes, TAR_HEADER_SIZE );
            rawBytes += TAR_HEADER_SIZE;
            //When a block with zeroes-only is found, the TAR archive ends here
            if ( memcmp( &currentFileHeader, zeroBlock, TAR_HEADER_SIZE ) == 0 ) {
                //cout << "Found TAR end\n";
                break;
            }
            //Uncomment this to check all header checksums
            //There seem to be TARs on the internet which include single headers that do not match the checksum even if most headers do.
            //This might indicate a code error.
            //assert(currentFileHeader.checkChecksum());
            //Uncomment this to check for USTAR if you need USTAR features
            //assert(currentFileHeader.isUSTAR());
            //Convert the filename to a std::string to make handling easier
            //Filenames of length 100+ need special handling
            // (only USTAR supports 101+-character filenames, but in non-USTAR archives the prefix is 0 and therefore ignored)
            std::string filename( currentFileHeader.filename,
                                  std::min((size_t) 100, strlen( currentFileHeader.filename )));
            //---Remove the next block if you don't want to support long filenames---
            size_t prefixLength = strlen( currentFileHeader.filenamePrefix );
            if ( prefixLength > 0 ) { //If there is a filename prefix, add it to the string. See `man ustar`LON
                filename = std::string( currentFileHeader.filenamePrefix, std::min((size_t) 155, prefixLength )) + "/" +
                           filename; //min limit: Not needed by spec, but we want to be safe
            }
            //Ignore directories, only handle normal files (symlinks are currently ignored completely and might cause errors)
            if ( currentFileHeader.typeFlag == '0' || currentFileHeader.typeFlag == 0 ) { //Normal file
                //Handle GNU TAR long filenames -- the current block contains the filename only whilst the next block contains metadata
                if ( nextEntryHasLongName ) {
                    //Set the filename from the current header
                    filename = std::string( currentFileHeader.filename );
                    //The next header contains the metadata, so replace the header before reading the metadata
                    std::memcpy((char *) &currentFileHeader, rawBytes, TAR_HEADER_SIZE );
                    rawBytes += TAR_HEADER_SIZE;
                    //Reset the long name flag
                    nextEntryHasLongName = false;
                }
                //Now the metadata in the current file header is valie -- we can read the values.
                size_t size = currentFileHeader.getFileSize();
                //Log that we found a file
//                cout << "Found file '" << filename << "' (" << size << " bytes)\n";
                //Read the file into memory
                //  This won't work for very large files -- use streaming methods there!

                // ### Needs a new callback to handle file loading!!

                auto *b = reinterpret_cast<const unsigned char *>(rawBytes);
                ret.emplace( filename, SerializableContainer{ b, b + size } );

                rawBytes += size;
                //In the tar archive, entire 512-byte-blocks are used for each file
                //Therefore we now have to skip the padded bytes.
                size_t paddingBytes = ( TAR_HEADER_SIZE - ( size % TAR_HEADER_SIZE )) %
                                      TAR_HEADER_SIZE; //How long the padding to 512 bytes needs to be
                //Simply ignore the padding
                rawBytes += paddingBytes;
                //----Remove the else if and else branches if you want to handle normal files only---
            } else if ( currentFileHeader.typeFlag == '5' ) { //A directory
                //Currently long directory names are not handled correctly
//                cout << "Found directory '" << filename << "'\n";
            } else if ( currentFileHeader.typeFlag == 'L' ) {
                nextEntryHasLongName = true;
            } else {
                //Neither normal file nor directory (symlink etc.) -- currently ignored silently
                std::cout << "Found unhandled TAR Entry type " << currentFileHeader.typeFlag << "\n";
            }
        }
        return ret;
    }

#define TARHEADER static_cast<PosixTarHeader*>(header)

    void TarWrite::_init( void *header ) {
        std::memset( header, 0, sizeof( PosixTarHeader ));
        std::strcpy( TARHEADER->magic, "ustar" );
        std::strcpy( TARHEADER->version, " " );
        std::sprintf( TARHEADER->mtime, "%011lo", time( NULL ));
        std::sprintf( TARHEADER->mode, "%07o", 0644 );
        const char *s = userComputerName().c_str();
        if ( s != NULL ) std::snprintf( reinterpret_cast<char *>(TARHEADER), 32, "%s", s );
        std::sprintf( TARHEADER->gname, "%s", "users" );
    }

    void TarWrite::_checksum( void *header ) {
        unsigned int sum = 0;
        char *p = (char *) header;
        char *q = p + sizeof( PosixTarHeader );
        while ( p < TARHEADER->checksum ) sum += *p++ & 0xff;
        for ( int i = 0; i < 8; ++i ) {
            sum += ' ';
            ++p;
        }
        while ( p < q ) sum += *p++ & 0xff;

        std::sprintf( TARHEADER->checksum, "%06o", sum );
    }

    void TarWrite::_size( void *header, unsigned long fileSize ) {
        std::sprintf( TARHEADER->size, "%011llo", (long long unsigned int) fileSize );
    }

    void TarWrite::_filename( void *header, const char *filename ) {
        if ( filename == NULL || filename[0] == 0 || std::strlen( filename ) >= 100 ) {
            LOGR( "invalid archive name %s", filename );
        }
        std::snprintf( TARHEADER->name, 100, "%s", filename );
    }

    void TarWrite::_endRecord( std::size_t len ) {
        char c = '\0';
        while (( len % sizeof( PosixTarHeader )) != 0 ) {
            out.write( &c, sizeof( char ));
            ++len;
        }
    }


    TarWrite::TarWrite( std::ostream& out ) : _finished( false ), out( out ) {
        if ( sizeof( PosixTarHeader ) != 512 ) {
            throw ( sizeof( PosixTarHeader ));
        }
    }

    TarWrite::~TarWrite() {
        if ( !_finished ) {
//            cerr << "[warning]tar file was not finished."<< endl;
        }
    }

/** writes 2 empty blocks. Should be always called before closing the Tar file */
    void TarWrite::finish() {
        _finished = true;
        //The end of the archive is indicated by two blocks filled with binary zeros
        PosixTarHeader header;
        std::memset((void *) &header, 0, sizeof( PosixTarHeader ));
        out.write((const char *) &header, sizeof( PosixTarHeader ));
        out.write((const char *) &header, sizeof( PosixTarHeader ));
        out.flush();
    }

    void TarWrite::put( const char *filename, const std::string& s ) {
        put( filename, s.c_str(), s.size());
    }

    void TarWrite::put( const char *filename, const char *content ) {
        put( filename, content, std::strlen( content ));
    }

    void TarWrite::put( const char *filename, const char *content, std::size_t len ) {
        PosixTarHeader header;
        _init((void *) &header );
        _filename((void *) &header, filename );
        header.typeflag[0] = 0;
        _size((void *) &header, len );
        _checksum((void *) &header );
        out.write((const char *) &header, sizeof( PosixTarHeader ));
        out.write( content, len );
        _endRecord( len );
    }

    std::string TarWrite::putFileHashing( const char *filename, const char *nameInArchive ) {
        auto buff = FM::readLocalFile(filename);
        if ( buff.second ) {
            put( nameInArchive, reinterpret_cast<const char *>(buff.first.get()), buff.second );
            return Hashable<>::hashOf( buff );
        } else {
            LOGR("Cannot open  %s", filename );
            return "";
        }
    }

    bool TarWrite::putFile(const char* filename,const char* nameInArchive)
    {
        char buff[BUFSIZ];
        std::FILE* in=std::fopen(filename,"rb");
        if(in==NULL)
        {
            LOGR("Cannot open  %s", filename );
            return false;
        }
        std::fseek(in, 0L, SEEK_END);
        long int len= std::ftell(in);
        std::fseek(in,0L,SEEK_SET);

        PosixTarHeader header;
        _init((void*)&header);
        _filename((void*)&header,nameInArchive);
        header.typeflag[0]=0;
        _size((void*)&header,len);
        _checksum((void*)&header);
        out.write((const char*)&header,sizeof(PosixTarHeader));

        std::size_t nRead=0;
        while((nRead=std::fread(buff,sizeof(char),BUFSIZ,in))>0)
        {
            out.write(buff,nRead);
        }
        std::fclose(in);

        _endRecord(len);
        return true;
    }

}