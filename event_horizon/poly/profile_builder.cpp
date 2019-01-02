#include "profile_builder.h"
#include <stb/stb_image.h>
#include "core/file_manager.h"
#include "profile.hpp"

bool ProfileBuilder::finalizaMake( DependencyMaker& _md, std::shared_ptr<Profile> profile ) {
    auto& sg = dynamic_cast<ProfileManager&>(_md);

    return sg.add( *this, profile );
}

bool ProfileBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, [[maybe_unused]] const DependencyStatus _status ) {

    std::shared_ptr<Profile> profile = std::make_shared<Profile>( Name(), std::move(_data));

    return finalizaMake( _md, profile );
}

bool ProfileBuilder::makeDirect( DependencyMaker& _md ) {

    if ( _md.exists( Name() ) ) return false;

    return finalizaMake( _md, cfunc( Name(), vv2fs, vfs ) );
}

bool ProfileBuilder::evaluateDirectBuild( DependencyMaker& _md ) {

    bool bMakeDirect = cfunc != nullptr;
    if ( bMakeDirect ) {
        makeDirect( _md );
    }

    return bMakeDirect;
}

ProfileBuilder::ProfileBuilder( const Vector2f& _v1, const Vector2f& _v2 ) {
    func( Profile::makeLine ).cv2( _v1 ).cv2( _v2 );
}

ProfileBuilder::ProfileBuilder( const Vector2f& _length ) {
    func( Profile::makeLine ).cv2( -_length*0.5f ).cv2( _length*0.5f );
}

ProfileBuilder::ProfileBuilder( float _radius, float _subDivs ) {
    func( Profile::makeWire).cf(_radius).cf(_subDivs);
}

bool ProfileManager::add( const ProfileBuilder& _pb, std::shared_ptr<Profile> _profile ) {
    profileList[_pb.Name()] = std::move( _profile );
    return true;
}
