//
// Created by Dado on 2018-12-27.
//

#include <chrono>
#include <iostream>
#include <sstream>
#include <random>
#include "uuid.hpp"

uint64_t UUIDGen::sGlobalCounter = 1;

std::string UUIDGen::make() {
    static bool hasBeenInitialized = false;
    static uint64_t seed = 0;
    static std::mt19937_64 rng;
    static std::uniform_int_distribution<int> unii;

    if ( !hasBeenInitialized ) {
        seed = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        // Generating random numbers with C++11's random requires an engine and a distribution.
        // This is an engine based on the Mersenne Twister 19937 (64 bits):
        rng = std::mt19937_64(seed);
        unii = std::uniform_int_distribution<int>(0, std::numeric_limits<int>::max());
        hasBeenInitialized = true;
    }

    auto t1 = std::chrono::system_clock::now().time_since_epoch().count();
    UUIDGen::sGlobalCounter++;

    std::stringstream ss{};
    ss << t1 << "-" << unii(rng) << "-" << UUIDGen::sGlobalCounter;
    return ss.str();
}