//
// Created by Dado on 2019-08-09.
//

#pragma once

#include <memory>
#include <string>
#include <sstream>

void createPlainLUT3D( int dimension, std::unique_ptr<uint8_t[]>& rawBytes ) {

    size_t totalSize = dimension * dimension * dimension * 3;
    rawBytes = std::make_unique<uint8_t[]>( totalSize );
    size_t off = 0;
    int row = 0;
    int blues = 0;
    while ( off < totalSize ) {
        rawBytes[off + 0] = static_cast<uint8_t>(((float) (( off / 3 ) % dimension ) / (float) ( dimension )) *
                                                 255.0f );
        rawBytes[off + 1] = static_cast<uint8_t>(((float) (( row ) % dimension ) / (float) ( dimension )) *
                                                 255.0f );
        rawBytes[off + 2] = static_cast<uint8_t>(((float) (( blues ) % dimension ) / (float) ( dimension )) *
                                                 255.0f );
        off += 3;
        if (( off / 3 ) % dimension == 0 ) ++row;
        if ( row % dimension == 0 ) row = 0;
        if (( off / 3 ) % ( dimension * dimension ) == 0 ) {
            ++blues;
        }
    }

}

std::unique_ptr<uint8_t[]> loadLUT3D( const unsigned char *_buffer, size_t _length, int& dimension ) {

    std::unique_ptr<uint8_t[]> rawBytes;

    std::string ftext{ _buffer, _buffer + _length };
    std::istringstream iss( ftext );

    std::string line;
    size_t off = 0;
    while ( std::getline( iss, line )) {
        if ( dimension != 0 && !line.empty()) {
            float red, green, blue;
            std::sscanf( line.data(), "%f %f %f", &red, &green, &blue );
            rawBytes[off + 0] = static_cast<uint8_t>( red   * 255.0f );
            rawBytes[off + 1] = static_cast<uint8_t>( green * 255.0f );
            rawBytes[off + 2] = static_cast<uint8_t>( blue  * 255.0f );
            off += 3;
        } else {
            std::string LUT_3D_SIZE = "LUT_3D_SIZE";
            if ( auto p = line.find( LUT_3D_SIZE, 0 ); p != std::string::npos ) {
                dimension = std::stoi( line.substr( p + LUT_3D_SIZE.size()));
                rawBytes = std::make_unique<uint8_t[]>( dimension * dimension * dimension * 3 );
            }
        }
    }

    return rawBytes;
}
