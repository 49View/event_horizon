//
// Created by Dado on 2019-01-20.
//

#include "streaming_mediator.hpp"

void StreamingMediator::push( const std::string& _name, const StreamingPacket& _packet ) {
    packets.emplace( _name, _packet );
}

StreamingPacket StreamingMediator::pop( const std::string& _name ) {

    if ( auto it = packets.find(_name); it != packets.end() ) {
        StreamingPacket ret = it->second;
        packets.erase(it);
        return ret;
    }
    return StreamingPacket{};
}

void StreamingMediator::update() {
    for ( auto& [k,v] : streams ) {
        v->advanceFrame();
    }

}