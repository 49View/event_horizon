//
// Created by Dado on 2019-02-16.
//

#pragma once

#include <core/serialization.hpp>
#include <core/http/webclient.h>
#include <core/boxable.hpp>
#include <core/hashable.hpp>
#include <core/taggable.hpp>
#include <core/http/basen.hpp>
#include <core/zlib_util.h>
#include <core/metadata.h>
#include <core/resources/resource_utils.hpp>

template < typename T,
           typename B = JMATH::AABB,
           typename N = std::string >
class Publisher : public ResourceVersioning<T>,
                  public virtual Taggable<N>,
                  public virtual Hashable<> {
public:
    ResourcePipeElement pipe( const std::string& _name,
                              const SerializableContainer& _raw,
                              const ResourceDependencyDict& _deps = {} ) {
        this->Name(_name);
        this->calcHash( _raw );
        return { this->Name(), this->Hash(), toMetaData( _raw, _deps ) };
    }

protected:

    std::string toMetaData( const SerializableContainer& _raw,
                            const ResourceDependencyDict& _deps ) const {
        MegaWriter writer;
        CoreMetaData cmd{ this->Name(),
                          ResourceVersioning<T>::Prefix(),
                          this->Hash(),
                          "",//T::GenerateThumbnail((T&)*this),
                          zlibUtil::rawb64gzip(_raw),
                          this->Tags(),
                          _deps };
        cmd.serialize(&writer);
        return writer.getString();
    }

    void publish( const SerializableContainer& _raw,
                  const ResourceDependencyDict& _deps,
                  ResponseCallbackFunc callback ) const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData(_raw, _deps), callback );
    }

private:
    SerializableContainerDict pipes;
};

