//
// Created by Dado on 2019-01-20.
//

#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

class StreamingPacket {
public:
    StreamingPacket() = default;
    StreamingPacket( int width, int height, const uint8_t *data ) : width( width ),
                                                                    height( height ),
                                                                    data( data ) {}

public:
    int width = 0;
    int height = 0;
    const uint8_t* data = nullptr;
};

class StreamingMediator {
public:
    void push( const std::string& _nane, const StreamingPacket& _packet );
    StreamingPacket pop( const std::string& _name );
private:
    std::unordered_map<std::string, StreamingPacket> packets;
};


