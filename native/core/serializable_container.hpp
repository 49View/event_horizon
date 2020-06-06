//
// Created by dado on 06/06/2020.
//

#pragma once

#include <string>

static inline SerializableContainer serializableContainerFromString( const std::string& s ) {
    return SerializableContainer{ s.data(), s.data() + s.size() };
}


