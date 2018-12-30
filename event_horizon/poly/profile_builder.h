#pragma once

#include <memory>
#include <string>
#include "core/util.h"
#include "core/math/vector2f.h"
#include "core/builders.hpp"
#include "core/image_constants.h"
#include <core/math/poly_utils.hpp>

struct ProfileBuilder;

class Profile;

using profileDirectMakeFunc = std::function<std::shared_ptr<Profile>(const std::string&, const std::vector<Vector2f>&, const std::vector<float>&)>;

class ProfileManager : public DependencyMaker {
public:
DEPENDENCY_MAKER_EXIST( profileList );
    bool add( const ProfileBuilder& _pb, std::shared_ptr<Profile> _profile );

    std::shared_ptr<Profile> get( const std::string& _key ) { return profileList[_key]; }

private:
    std::unordered_map<std::string, std::shared_ptr<Profile>> profileList;
};

struct RBUILDER( ProfileBuilder, profiles, svg, Binary, BuilderQueryType::Exact, 0 )

    ProfileBuilder( const Vector2f& _length );
    ProfileBuilder( const Vector2f& _v1, const Vector2f& _v2 );

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

    bool evaluateDirectBuild( DependencyMaker& _md ) override;
    bool makeDirect( DependencyMaker& _md );
private:
    bool finalizaMake( DependencyMaker& sg, std::shared_ptr<Profile> profile );

private:
    profileDirectMakeFunc cfunc = nullptr;
    std::vector<Vector2f> vv2fs;
    std::vector<float> vfs;
};
