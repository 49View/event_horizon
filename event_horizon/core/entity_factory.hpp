//
// Created by Dado on 2019-02-23.
//

#pragma once

#include <memory>
#include <core/zlib_util.h>

class EntityFactory {
public:
    template <typename T>
    static std::shared_ptr<T> create( uint8_p&& _data ) {
        auto ret = std::make_shared<T>();
        ret->deserialize(zlibUtil::inflateFromMemory( std::move(_data) ));
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> create( const SerializableContainer& _data ) {
        auto ret = std::make_shared<T>();
        ret->deserialize(_data);
        return ret;
    }

    template <typename T>
    static std::shared_ptr<T> clone( const std::shared_ptr<T> _elem ) {
        auto ret = std::make_shared<T>();
        ret->clone( *_elem.get() );
        return ret;
    }
};

using EF = EntityFactory;