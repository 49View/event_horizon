//
// Created by Dado on 2018-10-08.
//

#include "render_pin.h"

void RenderPin::create( const UIShapeType _st, const std::string& _name, std::shared_ptr<Camera> _cam ) {
    st = _st;
    name = _name;

    float fov = atanf( degToRad(_cam->FoV()) );
    float scale = 0.5f;
    auto vlist = { Vector3f{ sin( fov ), 0.0f, -cos( fov ) } * scale,
                   Vector3f::ZERO,
                   Vector3f{ -sin( fov ), 0.0f, -cos( fov ) } * scale };

    UISB{ st }.v( vlist ).vn( name ).build( rr );
}

void RenderPin::update( std::shared_ptr<Camera> _cam ) {
    rr.setRenderHook( name, renderHook );
    if ( auto rh = renderHook.lock()) {
        rh->mModelMatrix->setRotation( -_cam->quatAngle().y(), Vector3f::UP_AXIS );
        rh->mModelMatrix->setTranslation( _cam->getPosition() );
    }
}
