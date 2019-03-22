#include "profile_builder.h"
#include <poly/profile.hpp>

//bool ProfileBuilder::finalizaMake( std::shared_ptr<Profile> profile ) {
//    mm.add( profile );
//    return true;
//}

//bool ProfileBuilder::makeImpl( uint8_p&& _data, [[maybe_unused]] const DependencyStatus _status ) {
//
//    std::shared_ptr<Profile> profile = std::make_shared<Profile>( Name(), std::move(_data));
//
//    return finalizaMake( profile );
//}

//bool ProfileBuilder::makeDirect() {
//
//    if ( mm.exists( Name() ) ) return false;
//
//    return finalizaMake( cfunc( Name(), vv2fs, vfs ) );
//}

void ProfileBuilder::finalise( std::shared_ptr<Profile> _elem ) {

}

//bool ProfileBuilder::evaluateDirectBuild() {
//
//    bool bMakeDirect = cfunc != nullptr;
//    if ( bMakeDirect ) {
//        makeDirect();
//    }
//
//    return bMakeDirect;
//}
//
//ProfileBuilder::ProfileBuilder( ProfileManager& _pm, const Vector2f& _v1, const Vector2f& _v2 ) : ResourceBuilder( _pm ) {
//    func( Profile::makeLine ).cv2( _v1 ).cv2( _v2 );
//}
//
//ProfileBuilder::ProfileBuilder( ProfileManager& _pm, const Vector2f& _length ) : ResourceBuilder( _pm ) {
//    func( Profile::makeLine ).cv2( -_length*0.5f ).cv2( _length*0.5f );
//}
//
//ProfileBuilder::ProfileBuilder( ProfileManager& _pm, float _radius, float _subDivs ) : ResourceBuilder( _pm ) {
//    func( Profile::makeWire).cf(_radius).cf(_subDivs);
//}
