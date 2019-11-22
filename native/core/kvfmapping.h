//
// Created by Dado on 14/10/2017.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "htypes_shared.hpp"

using UIFSMappingFunc = std::function<std::string( const size_t )>;

class KVFMapping {
public:
    KVFMapping() {};
    KVFMapping( std::initializer_list<std::pair<std::string, std::string> > kv,
                std::initializer_list<std::pair<std::string, CommandCallbackFunction> > kfc,
                std::initializer_list<std::pair<std::string, UIFSMappingFunc> > kfa );
    std::string findValue( const std::string& _key ) const;
    UIFSMappingFunc findFunctionA( const std::string& _key ) const;
    CommandCallbackFunction findFunctionC( const std::string& _key ) const;
    void injectValue( std::string& _ret, const std::string& _key );
    CommandCallbackFunction injectCallback( const std::string& _key ) const;

    std::vector<std::string> injectValueArrayFromFunction( const std::string& _key );

    std::string& operator[]( const std::string& _valueName );
private:
    std::unordered_map<std::string, CommandCallbackFunction> kfcmapping;
    std::unordered_map<std::string, UIFSMappingFunc> kfamapping;
    std::unordered_map<std::string, std::string> kvmapping;
};
