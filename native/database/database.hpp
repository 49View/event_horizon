//
// Created by Dado on 09/12/2019.
//

#pragma once

#include <vector>
#include <string>

struct DBConnection {
    DBConnection() = default;
    DBConnection( const std::string& defaultDatabase ) : defaultDatabase( defaultDatabase ) {}
    DBConnection( const char* defaultDatabase ) : defaultDatabase( defaultDatabase ) {}

    std::string host = "localhost";
    unsigned int port = 0;
    std::vector<std::string> hosts;
    std::vector<unsigned int> ports;
    std::string defaultDatabase{};
    std::string replicaSet = "rs0";
};

