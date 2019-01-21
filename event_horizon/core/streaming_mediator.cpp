//
// Created by Dado on 2019-01-20.
//

#include "streaming_mediator.hpp"

void StreamingMediator::push( const std::string& _name, const uint8_t *_packet ) {
    packets.emplace( _name, _packet );
}

const uint8_t* StreamingMediator::pop( const std::string& _name ) {
    const uint8_t* ret = nullptr;
    if ( auto it = packets.find(_name); it != packets.end() ) {
        ret = it->second;
        packets.erase(it);
    }
    return ret;
}
