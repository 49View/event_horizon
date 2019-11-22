#pragma once

#include <core/serialization.hpp>

JSONDATA( ImageParamsJSONAble, width, height, depth, channels, bpp, compressedSize, uncompressedSize, headerType,
          outFormat, ttm, wrapMode, filterMode )

    ImageParamsJSONAble( int width, int height, int depth, int channels, int bpp, size_t compressedSize,
                         size_t uncompressedSize, const std::string& headerType, const std::string& outFormat,
                         const std::string& ttm, const std::string& wrapMode, const std::string& filterMode ) : width(
            width ), height( height ), depth( depth ), channels( channels ), bpp( bpp ), compressedSize(
            compressedSize ), uncompressedSize( uncompressedSize ), headerType( headerType ), outFormat( outFormat ),
                                                                                                                ttm( ttm ),
                                                                                                                wrapMode(
                                                                                                                        wrapMode ),
                                                                                                                filterMode(
                                                                                                                        filterMode ) {}

    int width = 0;
    int height = 0;
    int depth = 1;
    int channels = 3;
    int bpp = 8;
    uint64_t compressedSize = 0;
    uint64_t uncompressedSize = 0;
    std::string headerType;
    std::string outFormat;
    std::string ttm;
    std::string wrapMode;
    std::string filterMode;
};
