//
// Created by Dado on 2019-02-19.
//

#pragma once

#include <vector>

class SerializeBin;
class DeserializeBin;

template <typename D, typename W = SerializeBin, typename R = DeserializeBin>
class Serializable {
protected:
    void deserialize( const std::vector<char>& _data ) {
        deserialize( std::make_shared<R>(_data, D::Version() ) );
    }

    virtual void serialize( std::shared_ptr<W> writer ) const = 0;
    virtual void deserialize( std::shared_ptr<R> reader ) = 0;
};
