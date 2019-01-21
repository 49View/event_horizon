//
// Created by Dado on 2019-01-20.
//

#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

class StreamingMediator {
public:
    void push( const std::string& _nane, const uint8_t* _packet );
    const uint8_t* pop( const std::string& _name );
private:
    std::unordered_map<std::string, const uint8_t*> packets;
};


