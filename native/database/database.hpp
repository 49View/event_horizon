//
// Created by Dado on 09/12/2019.
//

#pragma once

#include <vector>
#include <string>

struct DBConnection {
    DBConnection( const std::string& path, const std::string& defaultDatabase, const std::string& replicaSet ) : path(
            path ), defaultDatabase( defaultDatabase ), replicaSet( replicaSet ) {}

    std::string path = "localhost:27017";
    std::string defaultDatabase{};
    std::string replicaSet = "rs0";
};

