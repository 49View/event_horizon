//
// Created by Dado on 2019-02-19.
//

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <core/serialize_types.hpp>

class SerializeBin;
class DeserializeBin;

template <typename W>
class Serialize {
public:
    virtual void serialize( std::shared_ptr<W> writer ) const = 0;
};

template <typename D, typename R>
class Deserialize {
protected:

    void deserialize( const std::vector<char>& _data ) {
        deserialize( std::make_shared<R>(_data, D::Version() ) );
    }

    void gatherDependencies( std::shared_ptr<R> reader ) {
        uint64_t numEntries = 0;

        reader->read( numEntries );
        while ( numEntries > 0 ) {
            reader->readDependency();
            --numEntries;
        }
    }
public:

    virtual void deserialize( std::shared_ptr<R> reader ) = 0;
};

template <typename D, typename W = SerializeBin, typename R = DeserializeBin>
class Serializable : public virtual Serialize<W>, public virtual Deserialize<D, R> {

};
