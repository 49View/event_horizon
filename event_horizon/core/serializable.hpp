//
// Created by Dado on 2019-02-19.
//

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

class SerializeBin;
class DeserializeBin;

template <typename W>
class Serialize {
public:
    virtual void serialize( std::shared_ptr<W> writer ) const = 0;
};

struct SerializeDependencyContainer {
    uint64_t        version;
    std::string     entityGroup;
    std::shared_ptr<Serialize<SerializeBin>> resource;
};

template <typename D, typename R>
class Deserialize {
protected:
    void deserialize( const std::vector<char>& _data ) {
        deserialize( std::make_shared<R>(_data, D::Version() ) );
    }
public:
    virtual void deserialize( std::shared_ptr<R> reader ) = 0;
};

template <typename D, typename W = SerializeBin, typename R = DeserializeBin>
class Serializable : public virtual Serialize<W>, public virtual Deserialize<D, R> {

};

using SerializeDependencyHash      = int64_t;
using SerializationDependencyMap   = std::map<SerializeDependencyHash, SerializeDependencyContainer>;
using DeserializationDependencyMap = std::map<SerializeDependencyHash, SerializeDependencyContainer>;
