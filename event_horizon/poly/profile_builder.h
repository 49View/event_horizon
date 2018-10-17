#pragma once

#include <memory>
#include <string>
#include "core/util.h"
#include "core/math/vector2f.h"
#include "core/callback_dependency.h"
#include "core/image_constants.h"

struct ProfileBuilder;

class Profile;

enum class CustomProfileType {
    None,
    WireFrame,
    InnerDoorFrame,
    WindowEdgePlaster
};

class ProfileManager : public DependencyMaker {
public:
DEPENDENCY_MAKER_EXIST( profileList );
    bool add( const ProfileBuilder& _pb, std::shared_ptr<Profile> _profile );

    std::shared_ptr<Profile> get( const std::string& _key ) { return profileList[_key]; }

private:
    std::unordered_map<std::string, std::shared_ptr<Profile>> profileList;
};

struct RBUILDER( ProfileBuilder, profiles, svg, Binary, BuilderQueryType::Exact )

    ProfileBuilder& ct( const CustomProfileType _v ) {
        customType = _v;
        return *this;
    }

    ProfileBuilder& cv2( const Vector2f& _v ) {
        vv2fs.emplace_back( _v );
        return *this;
    }

    ProfileBuilder& cf( const float _v ) {
        vfs.emplace_back( _v );
        return *this;
    }

    bool makeDirect( DependencyMaker& _md );
private:
    bool finalizaMake( DependencyMaker& sg, std::shared_ptr<Profile> profile );
    std::shared_ptr<Profile> makeInnerDoorFrameProfile();
    std::shared_ptr<Profile> makeWindowEdgePlaster();
    std::shared_ptr<Profile> makeWireFrame();

private:
//    PivotPointPosition ppp;
//    Vector2f customPivotPoint;

    // Custom profile builder params
    CustomProfileType customType = CustomProfileType::None;
    std::vector<Vector2f> vv2fs;
    std::vector<float> vfs;
};
