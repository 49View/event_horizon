//
// Created by Dado on 2019-02-16.
//

#pragma once

#include <core/serialization.hpp>
#include <core/http/webclient.h>
#include <core/name_policy.hpp>
#include <core/boxable.hpp>
#include <core/http/basen.hpp>
#include <core/zlib_util.h>
#include <core/serializable.hpp>

template <typename T, typename B = JMATH::AABB, typename W = SerializeBin, typename R = DeserializeBin>
class Publisher : public virtual NamePolicy<>, public virtual Boxable<B>, public virtual Serializable<T, W, R> {
protected:
    virtual std::string generateThumbnail() const = 0;
    virtual std::set<std::string> generateTags() const = 0;

    std::string generateRawData() const {
        auto writer = std::make_shared<W>(T::Version());
        this->serialize( writer );
        auto matFile = writer->buffer();

        auto f = zlibUtil::deflateMemory( std::string{ matFile.begin(), matFile.end() } );
        auto rawm = bn::encode_b64( f );
        return std::string{ rawm.begin(), rawm.end() };
    }

    void nameSplit( std::set<std::string>& ret ) const {
        auto lcname = toLower( Name() );
        auto ltags = split( lcname, '_' );
        for ( const auto& v : ltags ) {
            ret.emplace( v );
        }
    }

    std::string toMetaData() const {

        MegaWriter writer;

        writer.StartObject();
        writer.serialize( CoreMetaData{Name(), T::EntityGroup(), T::Version(),
                                       generateThumbnail(), generateRawData(), generateTags()} );
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
