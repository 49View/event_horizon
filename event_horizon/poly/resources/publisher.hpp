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
#include <core/serializebin.hpp>
#include <core/versionable.hpp>
#include <core/query_policy.hpp>

class Material;
class GeomData;
class Profile;
class ImageBuilder;
class FontBuilder;
class CameraRig;
class MaterialColor;

template <typename R>
class ResourceVersioning {
public:
    inline static constexpr bool usesNotExactQuery() {
        if ( std::is_same<R, Material>::value )        return true;
        if ( std::is_same<R, GeomData>::value )        return true;
        if ( std::is_same<R, MaterialColor>::value  )  return true;
        return false;
    }

    inline static size_t Version() {
        if ( std::is_same<R, Material>::value )       return 1000;
        if ( std::is_same<R, GeomData>::value )       return 2000;
        if ( std::is_same<R, Profile>::value  )       return 1000;
        if ( std::is_same<R, MaterialColor>::value  ) return 1000;
        return 0;
    }

    inline static std::string Prefix() {
        if ( std::is_same<R, Material>::value )                return "material";
        if ( std::is_same<R, GeomData>::value )                return "geom";

        if ( std::is_same<R, MaterialColor>::value  )          return "color";
        if ( std::is_same<R, CameraRig>::value )               return "cameras";

        if ( std::is_same<R, Profile>::value  )                return "profiles";
        if ( std::is_same<R, ImageBuilder>::value  )           return "image";
        if ( std::is_same<R, FontBuilder>::value  )            return "font";
        return "unknown";
    }

    inline static std::string GenerateThumbnail( const R& _res ) {
        if ( std::is_same<R, Material>::value )                return "material";
        if ( std::is_same<R, GeomData>::value )                return "geom";

        if ( std::is_same<R, MaterialColor>::value  )          return "color";
        if ( std::is_same<R, CameraRig>::value )               return "cameras";

        if ( std::is_same<R, Profile>::value  )                return "profiles";
        if ( std::is_same<R, ImageBuilder>::value  )           return "image";
        if ( std::is_same<R, FontBuilder>::value  )            return "font";
        return "unknown";
    }

};

template < typename T,
           typename B = JMATH::AABB,
           typename W = SerializeBin,
           typename R = DeserializeBin,
           typename N = std::string >
class Publisher : public ResourceVersioning<T>,
                  public virtual Boxable<B>,
                  public virtual Serializable<T, W, R>,
                  public virtual HashTaggable<N> {
protected:
    virtual void serializeInternal( std::shared_ptr<W> writer ) const = 0;
    virtual void deserializeInternal( std::shared_ptr<R> reader ) = 0;

    std::string rawb64gzip( const SerializableContainer& _raw ) const {
        auto f = zlibUtil::deflateMemory( std::string{ _raw.begin(), _raw.end() } );
        auto rawm = bn::encode_b64( f );
        return std::string{ rawm.begin(), rawm.end() };
    }

    std::string generateRawData() const {
        auto writer = std::make_shared<W>( SerializeHeader{ this->Hash(), T::Version(), T::Prefix() } );
        this->serialize( writer );
        auto matFile = writer->buffer();

        return rawb64gzip(matFile);
    }

    std::string toMetaData() const {
        MegaWriter writer;
        writer.StartObject();
        writer.serialize( CoreMetaData{ this->Name(), T::Prefix(),
                                        T::GenerateThumbnail((T&)*this), generateRawData(), this->Tags() } );
        if ( B::IsSerializable() ) {
            writer.serialize( "BBox3d", Boxable<B>::BBox3d() );
        }
        writer.EndObject();

        return writer.getString();
    }

    std::string toMetaData( const SerializableContainer& _raw ) const {
        MegaWriter writer;
        writer.StartObject();
        writer.serialize( CoreMetaData{ this->Name(),
                                        T::Prefix(),
                                        T::GenerateThumbnail((T&)*this),
                                        rawb64gzip(_raw),
                                        this->Tags() } );
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
        serializeInternal( writer );
    }

    void deserializeImpl( std::shared_ptr<R> reader ) override {
        reader->read( HashTaggable<N>::NameRef() );
        reader->read( HashTaggable<N>::HashRef() );
        if ( B::IsSerializable() ) {
            reader->read( Boxable<B>::BBox3d() );
        }
        deserializeInternal( reader );
    }

    void publish2( const SerializableContainer& _raw, ResponseCallbackFunc callback  ) const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData(_raw), callback );
    }

public:
    void publish() const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
    }
};
