#pragma once

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

/*
 * tarball.h (writer)
 *
 *  Created on: Jul 28, 2010
 *      Author: Pierre Lindenbaum PhD
 *              plindenbaum@yahoo.fr
 *              http://plindenbaum.blogspot.com
 *
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <core/htypes_shared.hpp>

namespace tarUtil {

    bool isTar( const SerializableContainer& fin );

//Check file extensions
#define ASCII_TO_NUMBER( num ) ((num)-48) //Converts an ascii digit to the corresponding number (assuming it is an ASCII digit)

/**
 * Decode a TAR octal number.
 * Ignores everything after the first NUL or space character.
 * @param data A pointer to a size-byte-long octal-encoded
 * @param size The size of the field pointer to by the data pointer
 * @return
 */
    static uint64_t decodeTarOctal( char *data, size_t size = 12 ) {
        unsigned char *currentPtr = (unsigned char *) data + size;
        uint64_t sum = 0;
        uint64_t currentMultiplier = 1;
        //Skip everything after the last NUL/space character
        //In some TAR archives the size field has non-trailing NULs/spaces, so this is neccessary
        unsigned char *checkPtr = currentPtr; //This is used to check where the last NUL/space char is
        for ( ; checkPtr >= (unsigned char *) data; checkPtr-- ) {
            if (( *checkPtr ) == 0 || ( *checkPtr ) == ' ' ) {
                currentPtr = checkPtr - 1;
            }
        }
        for ( ; currentPtr >= (unsigned char *) data; currentPtr-- ) {
            sum += ASCII_TO_NUMBER( *currentPtr ) * currentMultiplier;
            currentMultiplier *= 8;
        }
        return sum;
    }

    struct PosixTarHeader
    {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char checksum[8];
        char typeflag[1];
        char linkname[100];
        char magic[6];
        char version[2];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char pad[12];
    };

    struct TARFileHeader {
        char filename[100]; //NUL-terminated
        char mode[8];
        char uid[8];
        char gid[8];
        char fileSize[12];
        char lastModification[12];
        char checksum[8];
        char typeFlag; //Also called link indicator for none-UStar format
        char linkedFileName[100];
        //USTar-specific fields -- NUL-filled in non-USTAR version
        char ustarIndicator[6]; //"ustar" -- 6th character might be NUL but results show it doesn't have to
        char ustarVersion[2]; //00
        char ownerUserName[32];
        char ownerGroupName[32];
        char deviceMajorNumber[8];
        char deviceMinorNumber[8];
        char filenamePrefix[155];
        char padding[12]; //Nothing of interest, but relevant for checksum
        /**
         * @return true if and only if
         */
        bool isUSTAR() {
            return ( memcmp( "ustar", ustarIndicator, 5 ) == 0 );
        }

        /**
         * @return The filesize in bytes
         */
        size_t getFileSize() {
            return decodeTarOctal( fileSize );
        }

        /**
         * Return true if and only if the header checksum is correct
         * @return
         */
        bool checkChecksum() {
            //We need to set the checksum to zer
            char originalChecksum[8];
            std::memcpy( originalChecksum, checksum, 8 );
            std::memset( checksum, ' ', 8 );
            //Calculate the checksum -- both signed and unsigned
            uint64_t unsignedSum = 0;
            int64_t signedSum = 0;
            for ( size_t i = 0; i < sizeof( TARFileHeader ); i++ ) {
                unsignedSum += ((unsigned char *) this )[i];
                signedSum += ((signed char *) this )[i];
            }
            //Copy back the checksum
            std::memcpy( checksum, originalChecksum, 8 );
            //Decode the original checksum
            uint64_t referenceChecksum = decodeTarOctal( originalChecksum );
            return ( referenceChecksum == unsignedSum || ( static_cast<int64_t >(referenceChecksum) == signedSum ) );
        }
    };

    SerializableContainerDict untar( const SerializableContainer& fin );

    class TarWrite {
    private:
        bool _finished;
    protected:
        std::ostream& out;
        void _init(void* header);
        void _checksum(void* header);
        void _size(void* header,unsigned long fileSize);
        void _filename(void* header,const char* filename);
        void _endRecord(std::size_t len);
    public:
        explicit TarWrite(std::ostream& out);
        virtual ~TarWrite();
        /** writes 2 empty blocks. Should be always called before closing the Tar file */
        void finish();
        void put(const char* filename,const std::string& s);
        void put(const char* filename,const char* content);
        void put(const char* filename,const char* content,std::size_t len);
        void put( const std::string& filename, const SerializableContainer& _data );
        bool putFile(const char* filename,const char* nameInArchive);
        std::string putFileHashing( const char *filename, const char *nameInArchive );
    };
}
