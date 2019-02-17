//
// Created by Dado on 2019-02-16.
//

#pragma once

#include <core/name_policy.hpp>

template <typename T>
class Publisher : public virtual NamePolicy {
protected:
    virtual std::string generateThumbnail() const = 0;
    virtual std::set<std::string> generateTags() const = 0;
    virtual std::string generateRawData() const = 0;

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
        writer.EndObject();

        return writer.getString();
    }

public:
    void publish() const {
        Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
    }

};
