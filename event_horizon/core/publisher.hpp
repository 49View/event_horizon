//
// Created by Dado on 2019-02-16.
//

#pragma once

#include <core/serialization.hpp>
#include <core/http/webclient.h>
#include <core/boxable.hpp>
#include <core/hashtaggable.hpp>
#include <core/http/basen.hpp>
#include <core/zlib_util.h>
#include <core/serializable.hpp>

template < typename T,
           typename B = JMATH::AABB,
           typename W = SerializeBin,
           typename R = DeserializeBin,
           typename N = std::string >
class Publisher : public virtual Boxable<B>,
                  public virtual Serializable<T, W, R>,
                  public virtual HashTaggable<N> {
protected:
    virtual std::string generateThumbnail() const = 0;

    std::string generateRawData() const {
        auto writer = std::make_shared<W>( SerializeHeader{ Hashable::Hash(), T::Version(), T::EntityGroup() } );
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

    void serializeImpl( std::shared_ptr<W> writer ) const override {
        writer->write( HashTaggable<N>::Name() );
        writer->write( HashTaggable<N>::Hash() );
        if ( B::IsSerializable() ) {
            writer->write( Boxable<B>::BBox3d() );
        }
    }

    void deserializeImpl( std::shared_ptr<R> reader ) override {
        reader->read( HashTaggable<N>::NameRef() );
        reader->read( HashTaggable<N>::HashRef() );
        if ( B::IsSerializable() ) {
            reader->read( Boxable<B>::BBox3d() );
        }
    }

public:
    void publish() const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
    }
};
