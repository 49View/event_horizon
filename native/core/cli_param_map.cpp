//
// Created by dado on 19/06/2020.
//

#include "cli_param_map.hpp"
#include <core/string_util.h>


CLIParamMap::CLIParamMap( int argc, char **argv ) {
    for ( auto t = 0; t < argc; t++ ) {
        auto ret = split(argv[t], '=' );
        if ( ret.size() == 2 ) {
            params.emplace( ret[0], ret[1] );
        }
    }
}

std::optional<std::string> CLIParamMap::getParam( const std::string &key ) const {
    if ( auto it = params.find(key); it != params.end() ) {
        return it->second;
    }
    return std::nullopt;
}

std::string CLIParamMap::printAll() const {
    std::string ret;
    for ( const auto& v : params ) {
        ret+=v.first+"="+v.second+"\n";
    }
    return ret;
}
