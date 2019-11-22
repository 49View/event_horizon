//
// Created by Dado on 2019-01-20.
//

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include <core/math/vector2i.h>

using AVInitCallback = std::function<void(const std::string&, const V2i&)>;

class AudioVideoStream {
public:
    virtual void advanceFrame() = 0;
    virtual V2i streamDim() const = 0;
    virtual const std::string& Name() const = 0;
};

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
    template <typename T>
    void addStream( const std::string& _streamName, AVInitCallback avc ) {
        auto stream = std::make_shared<T>( _streamName, *this, avc );
        streams.emplace( _streamName, stream );
    }

    void update();

    void push( const std::string& _nane, const StreamingPacket& _packet );
    StreamingPacket pop( const std::string& _name );
private:
    std::unordered_map<std::string, StreamingPacket> packets;
    std::unordered_map<std::string, std::shared_ptr<AudioVideoStream>> streams;
};


