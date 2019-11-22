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

class UUIDIntegerInc {
public:
    explicit UUIDIntegerInc( uint64_t _base = 0 );

    [[nodiscard]] uint64_t getUUIntegerId() const { return mIntegerId; }
private:
    uint64_t mIntegerId = 0;
    static uint64_t SGlobalIntegerIncCounter;
};