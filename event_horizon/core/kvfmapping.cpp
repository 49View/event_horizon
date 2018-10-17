//
// Created by Dado on 14/10/2017.
//

#include "kvfmapping.h"

std::string KVFMapping::findValue( const std::string& _key ) const {

    if ( auto ret = kvmapping.find( _key ); ret != kvmapping.end()) {
        return ret->second;
    }
    return "";
}

KVFMapping::KVFMapping( std::initializer_list<std::pair<std::string, std::string> > kv,
                        std::initializer_list<std::pair<std::string, CommandCallbackFunction> > kfc,
                        std::initializer_list<std::pair<std::string, UIFSMappingFunc> > kfa ) {
    for ( auto& v : kv ) { kvmapping.emplace( v ); }
    for ( auto& v : kfa ) { kfamapping.emplace( v ); }
    for ( auto& v : kfc ) { kfcmapping.emplace( v ); }
}

UIFSMappingFunc KVFMapping::findFunctionA( const std::string& _key ) const {
    if ( auto ret = kfamapping.find( _key ); ret != kfamapping.end()) {
        return ret->second;
    }
    return nullptr;
}

CommandCallbackFunction KVFMapping::findFunctionC( const std::string& _key ) const {
    if ( auto ret = kfcmapping.find( _key ); ret != kfcmapping.end()) {
        return ret->second;
    }
    return nullptr;
}

void KVFMapping::injectValue( std::string& _ret, const std::string& _key ) {
    if ( auto string_inj = findValue( _key ); !string_inj.empty()) {
        _ret = string_inj;
    }
}

CommandCallbackFunction KVFMapping::injectCallback( const std::string& _key ) const {
    if ( _key.empty() ) return nullptr;
    if ( auto cb_inj = findFunctionC( _key ); cb_inj ) {
        return cb_inj;
    }
    return nullptr;
}

std::vector<std::string> KVFMapping::injectValueArrayFromFunction( const std::string& _key ) {
    std::vector<std::string> ret;

    auto s = findFunctionA( _key );
    if ( s != nullptr ) {
        auto aCount = 0;
        std::string text{};
        while ( !( text = s( aCount++ )).empty()) {
            ret.push_back( text );
        }
    } else {
        ret.push_back( _key );
    }

    return ret;
}

std::string& KVFMapping::operator[]( const std::string& _valueName ) {
    return kvmapping[_valueName];
}
