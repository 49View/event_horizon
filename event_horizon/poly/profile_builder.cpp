#include "profile_builder.h"
#include <stb/stb_image.h>
#include "core/file_manager.h"
#include "profile.hpp"

bool ProfileBuilder::finalizaMake( DependencyMaker& _md, std::shared_ptr<Profile> profile ) {
    ProfileManager& sg = static_cast<ProfileManager&>(_md);

    return sg.add( *this, profile );
}

bool ProfileBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    std::shared_ptr<Profile> profile = std::make_shared<Profile>( Name(), std::move(_data));

    return finalizaMake( _md, profile );
}

bool ProfileBuilder::makeDirect( DependencyMaker& _md ) {

    if ( _md.exists( Name() ) ) return false;

    std::shared_ptr<Profile> profile;
    switch ( customType ) {
        case CustomProfileType::None:
            return false;
            break;
        case CustomProfileType::InnerDoorFrame:
            profile = makeInnerDoorFrameProfile();
            break;
        case CustomProfileType::WindowEdgePlaster:
            profile = makeWindowEdgePlaster();
            break;
        case CustomProfileType::WireFrame:
            profile = makeWireFrame();
            break;
    }

    return finalizaMake( _md, profile );
}

bool ProfileManager::add( const ProfileBuilder& _pb, std::shared_ptr<Profile> _profile ) {
    profileList[_pb.Name()] = _profile;
    return true;
}

// Custom profiles

std::shared_ptr<Profile> ProfileBuilder::makeInnerDoorFrameProfile() {

    Vector2f bump = vv2fs[0];
    float thickness = vfs[0];
    float th = thickness * -0.5f;
    float trim = vfs[1];
    float doorGeomThickness = vfs[2];

    std::vector<Vector2f> points;
    // Counterclockwise
    float yMiddleMin = thickness - doorGeomThickness - bump.y();
    float yMiddleMax = yMiddleMin + bump.y();
    points.push_back( Vector2f( 0.0f, thickness + th ));
    points.push_back( Vector2f( trim, thickness + th  ));
    points.push_back( Vector2f( trim, yMiddleMax + th  ));
    points.push_back( Vector2f( trim + bump.x(), yMiddleMax + th  ));
    points.push_back( Vector2f( trim + bump.x(), yMiddleMin + th  ));
    points.push_back( Vector2f( trim, yMiddleMin + th  ));
    points.push_back( Vector2f( trim, 0.0f + th  ));
    points.push_back( Vector2f( 0.0f, 0.0f + th  ));

    std::shared_ptr<Profile> doorFrame_profile = std::make_shared<Profile>();
    doorFrame_profile->createArbitrary( Vector2f( trim + bump.x(), thickness ), points,
                                       PivotPointPosition::PPP_TOP_LEFT );
    //	doorFrame_profile.mirror(Vector2f::X_AXIS);

    return doorFrame_profile;
}

std::shared_ptr<Profile> ProfileBuilder::makeWindowEdgePlaster() {

    ASSERT(vv2fs.size() == 2);
    std::shared_ptr<Profile> profile = std::make_shared<Profile>();
    profile->createLine( vv2fs[0], vv2fs[1] );

    return profile;
}

std::shared_ptr<Profile> ProfileBuilder::makeWireFrame() {

    ASSERT(vfs.size() == 1);
    std::shared_ptr<Profile> profile = std::make_shared<Profile>();

    profile->createWire( vfs[0], 3 );

    return profile;
}
