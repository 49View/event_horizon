#pragma once

#include <zlib.h>
#include "util.h"

namespace zlibUtil {
    // Decompress
    std::vector<char> inflateFromMemory( uint8_p&& fin );
    // Compress
    std::vector<unsigned char> deflateMemory( const uint8_p&& source, int level = Z_DEFAULT_COMPRESSION );
    std::vector<unsigned char> deflateMemory( const std::string& source, int level = Z_DEFAULT_COMPRESSION );
}