#include <algorithm>
#include <vector>
#include <cstring>
#include "zlib_util.h"

#include <core/http/basen.hpp>

namespace zlibUtil {

#define CHUNK 16384

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
    SerializableContainer inflateFromMemory( uint8_p&& fin ) {
        SerializableContainer inflatedVector;
        int ret;
        unsigned have;
        z_stream strm;
        unsigned char *in;
        unsigned char out[CHUNK];

        /* allocate inflate state */
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        ret = inflateInit( &strm );
        ASSERT( ret == Z_OK );
        int bytesRead = 0;

        /* decompress until deflate stream ends or end of file */
        do {
            strm.avail_in = fin.second > static_cast<long long unsigned int>(bytesRead) ? std::min((int) CHUNK, (int)
            ( fin.second - bytesRead )) : 0;
            in = fin.first.get() + bytesRead;
            if ( strm.avail_in == 0 )
                break;
            strm.next_in = in;

            /* run inflate() on input until output buffer not full */
            do {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = inflate( &strm, Z_NO_FLUSH );
                ASSERT( ret != Z_STREAM_ERROR );  /* state not clobbered */
                switch (ret) {
                    case Z_NEED_DICT:
                        ret = Z_DATA_ERROR;     /* and fall through */
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        (void)inflateEnd(&strm);
                        return inflatedVector;
                }
                have = CHUNK - strm.avail_out;
                for ( unsigned q = 0; q < have; q++ ) {
                    inflatedVector.push_back( out[q] );
                }
            } while ( strm.avail_out == 0 );

            bytesRead += CHUNK;

            /* done when inflate() says it's done */
        } while ( ret != Z_STREAM_END );

        ASSERT( ret == Z_STREAM_END );
        /* clean up and return */
        (void) inflateEnd( &strm );
        return inflatedVector;
    }

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
    SerializableContainer deflateMemory( const uint8_p&& source, int level )
    {
        SerializableContainer deflatedMemory;
        int ret, flush;
        unsigned have;
        z_stream strm;
        unsigned char in[CHUNK];
        unsigned char out[CHUNK];

        /* allocate deflate state */
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        ret = deflateInit(&strm, level);
        if (ret != Z_OK) {
            LOGR("Cannot initilalize deflate");
            return deflatedMemory;
        }

        uInt streamOffset = 0;
        /* compress until end of file */
        do {
            uInt chunkSize = std::min( (uint64_t )CHUNK, source.second - streamOffset );
            memcpy(in, source.first.get() + streamOffset, chunkSize );
            strm.avail_in = chunkSize;  //fread(in, 1, CHUNK, source);
            strm.next_in = in;

//        if (ferror(source)) {
//            (void)deflateEnd(&strm);
//            return Z_ERRNO;
//        }
            streamOffset += chunkSize;
            flush = streamOffset >= source.second ? Z_FINISH : Z_NO_FLUSH;

            /* run deflate() on input until output buffer not full, finish
               compression if all of source has been read in */
            do {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = deflate(&strm, flush);    /* no bad return value */
                ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
                have = CHUNK - strm.avail_out;
                for ( unsigned q = 0; q < have; q++ ) {
                    deflatedMemory.push_back( out[q] );
                }
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)deflateEnd(&strm);
//                return Z_ERRNO;
//            }
            } while (strm.avail_out == 0);
            ASSERT(strm.avail_in == 0);     /* all input will be used */

            /* done when last data in file processed */
        } while (flush != Z_FINISH);
        ASSERT(ret == Z_STREAM_END);        /* stream will be complete */

        /* clean up and return */
        (void)deflateEnd(&strm);

        return deflatedMemory;
    }

    SerializableContainer deflateMemory( const std::string& source, int level )
    {
        SerializableContainer deflatedMemory;
        int ret, flush;
        unsigned have;
        z_stream strm;
        unsigned char in[CHUNK];
        unsigned char out[CHUNK];

        /* allocate deflate state */
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        ret = deflateInit(&strm, level);
        if (ret != Z_OK) {
            LOGR("Cannot initilalize deflate");
            return deflatedMemory;
        }

        uInt streamOffset = 0;
        /* compress until end of file */
        do {
            uInt chunkSize = std::min( (size_t)CHUNK, source.size() - streamOffset );
            memcpy(in, source.c_str() + streamOffset, chunkSize );
            strm.avail_in = chunkSize;  //fread(in, 1, CHUNK, source);
            strm.next_in = in;

//        if (ferror(source)) {
//            (void)deflateEnd(&strm);
//            return Z_ERRNO;
//        }
            streamOffset += chunkSize;
            flush = streamOffset >= source.size() ? Z_FINISH : Z_NO_FLUSH;

            /* run deflate() on input until output buffer not full, finish
               compression if all of source has been read in */
            do {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = deflate(&strm, flush);    /* no bad return value */
                ASSERT(ret != Z_STREAM_ERROR);  /* state not clobbered */
                have = CHUNK - strm.avail_out;
                for ( unsigned q = 0; q < have; q++ ) {
                    deflatedMemory.push_back( out[q] );
                }
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)deflateEnd(&strm);
//                return Z_ERRNO;
//            }
            } while (strm.avail_out == 0);
            ASSERT(strm.avail_in == 0);     /* all input will be used */

            /* done when last data in file processed */
        } while (flush != Z_FINISH);
        ASSERT(ret == Z_STREAM_END);        /* stream will be complete */

        /* clean up and return */
        (void)deflateEnd(&strm);

        return deflatedMemory;
    }

    std::string rawb64gzip( const SerializableContainer& _raw ) {
        auto f = zlibUtil::deflateMemory( std::string{ _raw.begin(), _raw.end() } );
        auto rawm = bn::encode_b64( f );
        return std::string{ rawm.begin(), rawm.end() };
    }

}