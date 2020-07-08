//
// Created by Dado on 2019-02-23.
//

#pragma once

#include <memory>
#include <core/zlib_util.h>
#include <core/hashing/incremental_hash.hpp>

class EntityFactory {
public:
    template <typename T>
    static std::shared_ptr<T> create( const std::string& _name ) {
        auto ret = std::make_shared<T>(_name);
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> create( uint8_p&& _data ) {
        auto ret = std::make_shared<T>();
        ret->deserialize(zlibUtil::inflateFromMemory( std::move(_data) ));
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> create( const SerializableContainer& _data ) {
        return std::make_shared<T>(_data);
    }

    template <typename T, typename ...Args>
    static std::shared_ptr<T> create( Args&& ... args ) {
        return std::make_shared<T>( std::forward<Args>( args )... );
    }

    template <typename T>
    static std::shared_ptr<T> clone( const std::shared_ptr<T> _elem ) {
        auto ret = std::make_shared<T>(*_elem.get());
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> clone( const T& _elem ) {
        return std::make_shared<T>(_elem);
    }

    template <typename T>
    static std::shared_ptr<T> cloneHashed( const std::shared_ptr<T> _elem ) {
        auto ret = std::make_shared<T>(*_elem.get());
        ret->hash = HashInc();
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> cloneHashed( const T& _elem ) {
        auto ret = std::make_shared<T>(_elem);
        ret->hash = HashInc();
        return ret;
    }

    template <typename T>
    static T copy( const T& _elem ) {
        return T{_elem.serialize()};
    }

    template <typename T>
    static T copy( const std::shared_ptr<T> _elem ) {
        return T{_elem->serialize()};
    }

};

using EF = EntityFactory;