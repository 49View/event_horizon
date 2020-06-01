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
public:
    size_t count() const { return mData.size(); }

    static std::string calcTotalHash( const StringUniqueCollection& _names ) {
        std::string totalHash{};
        for ( const auto& lName : _names ) {
            if ( !lName.empty() ) {
                totalHash+="@"+lName;
            }
        }
        return totalHash;
    }

    void add( const StringUniqueCollection& _names, std::shared_ptr<T> _elem ) {
        auto totalHash = calcTotalHash( _names );

        if ( !totalHash.empty() ) {
            for ( const auto& lName : _names ) {
                if ( !lName.empty() ) {
                    mData.emplace( lName, _elem );
                }
            }
            mHashDataMap[totalHash] = _elem;
        }
    }

    [[nodiscard]] bool exists( const StringUniqueCollection& _names ) const {
        return mHashDataMap.find(calcTotalHash( _names )) != mHashDataMap.end();
    }

    [[nodiscard]] std::shared_ptr<T> find( const StringUniqueCollection& _names ) {
        if ( auto it = mHashDataMap.find(calcTotalHash( _names )); it != mHashDataMap.end() ) {
            return it->second;
        }
        return nullptr;
    }

    void remove( const std::string& _hash ) {
        if ( auto it = mHashDataMap.find(_hash); it != mHashDataMap.end() ) {
            erase_if( mData, [&, it](const auto& elem) {
                return elem == it->second;
            } );
            mHashDataMap.erase(it);
        }
    }

    inline std::shared_ptr<T> getFromHash( const std::string& _hash ) {
        return mData[_hash];
    }

    std::shared_ptr<T> get( const std::string& _id ) {
        return getFromHash(_id);
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
    HSMap    mHashDataMap;
};
