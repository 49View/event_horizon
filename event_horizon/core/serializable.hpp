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
    void serialize( std::shared_ptr<W> writer ) const {
        serializeDependenciesImpl( writer );
        writer->finalizeDeps();
        serializeImpl( writer );
    }

protected:
    virtual void serializeImpl( std::shared_ptr<W> writer ) const = 0;
    virtual void serializeDependenciesImpl( std::shared_ptr<W> writer ) const {}
};

template <typename D, typename R>
class Deserialize {
protected:
    void deserialize( const SerializableContainer& _data ) {
        auto reader = std::make_shared<R>(_data, D::Version() );
        gatherDependencies( reader );
        deserializeImpl( reader );
    };

    void gatherDependencies( std::shared_ptr<R> reader ) {
        uint64_t numEntries = 0;

        reader->read( numEntries );
        while ( numEntries > 0 ) {
            reader->readDependency();
            --numEntries;
        }
    }

    virtual void deserializeImpl( std::shared_ptr<R> reader ) = 0;

    friend class EntityFactory;
};

template <typename D, typename W = SerializeBin, typename R = DeserializeBin>
class Serializable : public virtual Serialize<W>, public virtual Deserialize<D, R> {
    using Deserialize<D,R>::Deserialize;
};
