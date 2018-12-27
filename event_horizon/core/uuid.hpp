//
// Created by Dado on 2018-12-27.
//

#pragma once

#include <cstdint>
#include <string>

class UUIDGen {
public:
    static std::string make();
private:
    static uint64_t sGlobalCounter;
};

using UUID = std::string;

