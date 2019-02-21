//
// Created by Dado on 2019-02-16.
//

#pragma once

#include <core/serialization.hpp>
#include <core/http/webclient.h>
#include <core/boxable.hpp>
#include <core/taggable.hpp>
#include <core/http/basen.hpp>
#include <core/zlib_util.h>
#include <core/serializable.hpp>

template < typename T,
           typename B = JMATH::AABB,
           typename W = SerializeBin,
           typename R = DeserializeBin,
           typename N = std::string >
class Publisher : public virtual Taggable<N>,
                  public virtual Boxable<B>,
                  public virtual Serializable<T, W, R> {
protected:
    virtual std::string generateThumbnail() const = 0;

    std::string generateRawData() const {
        auto writer = std::make_shared<W>(T::Version(), T::EntityGroup());
        this->serialize( writer );
        auto matFile = writer->buffer();

        auto f = zlibUtil::deflateMemory( std::string{ matFile.begin(), matFile.end() } );
        auto rawm = bn::encode_b64( f );
        return std::string{ rawm.begin(), rawm.end() };
    }

    std::string toMetaData() const {

        MegaWriter writer;

        writer.StartObject();
        writer.serialize( CoreMetaData{ this->Name(), T::EntityGroup(), T::Version(),
                                        generateThumbnail(), generateRawData(), this->Tags() } );
        if ( B::IsSerializable() ) {
            writer.serialize( "BBox3d", Boxable<B>::BBox3d() );
        }
        writer.EndObject();

        return writer.getString();
    }

public:
    void publish() const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
    }
};
