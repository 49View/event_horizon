#pragma once

#include <memory>
#include <string>
#include "core/util.h"
#include "core/math/vector2f.h"
#include "poly/resources/builders.hpp"
#include "core/image_constants.h"
#include <core/math/poly_utils.hpp>

class ProfileBuilder;

class Profile;

using profileDirectMakeFunc = std::function<std::shared_ptr<Profile>(const std::string&, const std::vector<Vector2f>&, const std::vector<float>&)>;

class ProfileManager : public ResourceManager<Profile> {
public:
    virtual ~ProfileManager() = default;

//    bool add( const ProfileBuilder& _pb, std::shared_ptr<Profile> _profile );
};

//    struct RBUILDER( ProfileBuilder, profiles, svg, Binary, BuilderQueryType::Exact, 0 )

class ProfileBuilder : public ResourceBuilder<Profile, ProfileManager> {
public:
    using ResourceBuilder::ResourceBuilder;

    // This will create a circular profile
    explicit ProfileBuilder( ProfileManager& _pm, float _radius, float _subDivs = 3 );
    explicit ProfileBuilder( ProfileManager& _pm, const Vector2f& _length );
             ProfileBuilder( ProfileManager& _pm, const Vector2f& _v1, const Vector2f& _v2 );

    ProfileBuilder& func( profileDirectMakeFunc _cf ) {
        cfunc = _cf;
        return *this;
    }

    ProfileBuilder& cv2( const Vector2f& _v ) {
        vv2fs.emplace_back( _v );
        Name( Name() + _v.toString() );
        return *this;
    }

    ProfileBuilder& cf( const float _v ) {
        Name( Name() + std::to_string(_v) );
        vfs.emplace_back( _v );
        return *this;
    }

    bool evaluateDirectBuild() override;
    bool makeDirect();
protected:
    bool finalizaMake( std::shared_ptr<Profile> profile );

private:
    profileDirectMakeFunc cfunc = nullptr;
    std::vector<Vector2f> vv2fs;
    std::vector<float> vfs;
};
