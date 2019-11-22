//
// Created by Dado on 2019-04-06.
//

#pragma once

#include <string>
#include <unordered_map>
#include <core/htypes_shared.hpp>

template <typename T>
class HashSharedMap {
    using  HSMap = std::unordered_map<std::string, std::shared_ptr<T>>;
    using  HSMaper = std::unordered_map<std::string, std::string>;
public:
    size_t count() const { return mData.size(); }

    void add( const StringUniqueCollection& _names, std::shared_ptr<T> _elem ) {
        for ( const auto& lName : _names )
            if ( !lName.empty() ) mData.emplace( lName, _elem );
    }

    inline std::shared_ptr<T> getFromHash( const std::string& _hash ) {
        return mData[_hash];
    }

    inline std::shared_ptr<T> getFromName( const std::string& _name ) {
        return mData[mMapper[_name]];
    }

    std::shared_ptr<T> get( const std::string& _id ) {
        auto ret = getFromHash(_id);
        if ( !ret ) {
            return getFromName(_id);
        }
        return ret;
    }

    auto begin() {
        return mData.begin();
    }

    auto end() {
        return mData.end();
    }

    auto begin() const {
        return mData.cbegin();
    }

    auto end() const {
        return mData.cend();
    }

protected:
    HSMap    mData;
    HSMaper  mMapper;
};
