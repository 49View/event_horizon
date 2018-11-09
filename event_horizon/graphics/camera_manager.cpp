#include "camera_manager.h"
#include "core/service_factory.h"
#include "core/app_globals.h"
#include "graphic_functions.hpp"
#include "framebuffer.h"
//#include "vr_manager.hpp"

CameraRig::CameraRig( const std::string& _name, const Rect2f& _viewport ) {
    mName = _name;
    mCamera = std::make_shared<Camera>( _name, CameraState::Active, _viewport );
    mCameraVRLeftEye = std::make_shared<Camera>( _name + "LeftEye", CameraState::Active, _viewport );
    mCameraVRRightEye = std::make_shared<Camera>( _name + "RightEye", CameraState::Active, _viewport );

    mViewport = _viewport;
}

CameraRig::CameraRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
    ASSERT(_fb != nullptr);
    mName = _name;
    mViewport = Rect2f{ Vector2f::ZERO, {_fb->getWidth(), _fb->getHeight()} };
    mFramebuffer = _fb;

    mCamera = std::make_shared<Camera>( _name, CameraState::Active, mViewport );
    mCameraVRLeftEye = std::make_shared<Camera>( _name + "LeftEye", CameraState::Active, mViewport );
    mCameraVRRightEye = std::make_shared<Camera>( _name + "RightEye", CameraState::Active, mViewport );
}

void CameraRig::setFramebuffer( std::shared_ptr<Framebuffer> framebuffer ) {
    mViewport = Rect2f{ Vector2f::ZERO, {framebuffer->getWidth(), framebuffer->getHeight()} };
    mFramebuffer = framebuffer;

//    mCamera->ViewPort( mViewport );
//    mCameraVRLeftEye->ViewPort( mViewport );
//    mCameraVRRightEye->ViewPort( mViewport );
}

std::shared_ptr<Camera> CameraRig::getCamera() {
//    if ( VRM.IsOn()) {
//        return CurrEye() == 0 ? mCameraVRLeftEye : mCameraVRRightEye;
//    }
    return mCamera;
}

bool CameraRig::contains( const Vector2f& pos ) const {
    return mViewport.contains( pos );
}

Vector2f CameraRig::mousePickRayOrtho( const Vector2f& _pos ) {
    return getMainCamera()->mousePickRayOrtho( _pos );
}


void CameraManager::syncCameraMovements( const std::string& cameraSource, const std::string& cameraDest ) {
    mSyncedRigs.push_back( std::make_pair( cameraSource, cameraDest ));
}

void CameraManager::desyncCameraMovements( const std::string& cameraSource, const std::string& cameraDest ) {
    std::pair<std::string, std::string> toFind = std::make_pair( cameraSource, cameraDest );
    mSyncedRigs.erase(
            remove_if( mSyncedRigs.begin(), mSyncedRigs.end(), [&]( auto const& us ) -> bool { return us == toFind; } ),
            mSyncedRigs.end());
}

void CameraManager::enableInputs( const bool _bEnable ) {
    for ( auto& c : mCameraRigs ) {
        c.second->getMainCamera()->enableInputs( _bEnable );
    }
}

bool CameraManager::isDoom() const {
    return getMainCameraMode() == CameraMode::Doom;
}

void CameraManager::update() {
    for ( auto& c : mSyncedRigs ) {
        getCamera( c.second )->setPosition( getCamera( c.first )->getPosition());
        getCamera( c.second )->setQuatAngles( getCamera( c.first )->quatAngle());
        getCamera( c.second )->setFoV( getCamera( c.first )->FoV());
        getCamera( c.second )->ViewPort( getCamera( c.first )->ViewPort());
    }

    for ( auto& c : mCameraRigs ) {
        c.second->getMainCamera()->update();
        // Copy camera values to left and right camera for VR
        c.second->getVRLeftCamera()->setPosition( c.second->getMainCamera()->getPosition());
        c.second->getVRLeftCamera()->setQuatAngles( c.second->getMainCamera()->quatAngle());
        c.second->getVRLeftCamera()->setFoV( c.second->getMainCamera()->FoV());
        c.second->getVRLeftCamera()->ViewPort( c.second->getMainCamera()->ViewPort());
        c.second->getVRRightCamera()->setPosition( c.second->getMainCamera()->getPosition());
        c.second->getVRRightCamera()->setQuatAngles( c.second->getMainCamera()->quatAngle());
        c.second->getVRRightCamera()->setFoV( c.second->getMainCamera()->FoV());
        c.second->getVRRightCamera()->ViewPort( c.second->getMainCamera()->ViewPort());

//        VRM.preRenderEye( *this, 0, c.first );
//        VRM.preRenderEye( *this, 1, c.first );
    }
}

void CameraManager::addCubeMapRig( Renderer& rr, const CameraCubeMapRigBuilder& _builder ) {
    const Rect2f cubemapViewport{ Vector2f::ZERO, {_builder.size, _builder.size} };

    auto cbfb = FrameBufferBuilder{ rr, _builder.name}.format(_builder.format).size(_builder.size)
                                                 .cubemap().mipMaps(_builder.useMipMaps).build();

    float cubeMapFOV = 90.0f;//degToRad(90.0f);
    auto ctop = addRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Top), cbfb );
    ctop->getCamera()->setFoV( cubeMapFOV );
    ctop->getCamera()->setPosition( _builder.pos );
    ctop->getCamera()->setQuatAngles( Vector3f{ M_PI_2, 0.0f, 0.0f } );
    ctop->getCamera()->Mode( CameraMode::Doom );
    ctop->getCamera()->ViewPort( cubemapViewport );
    ctop->getCamera()->enableInputs(false);

    auto cbottom = addRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Bottom), cbfb );
    cbottom->getCamera()->setFoV( cubeMapFOV );
    cbottom->getCamera()->setPosition( _builder.pos );
    cbottom->getCamera()->setQuatAngles( Vector3f{ -M_PI_2, 0.0f, 0.0f } );
    cbottom->getCamera()->Mode( CameraMode::Doom );
    cbottom->getCamera()->ViewPort( cubemapViewport );
    cbottom->getCamera()->enableInputs(false);

    auto cleft = addRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Left), cbfb );
    cleft->getCamera()->setFoV( cubeMapFOV );
    cleft->getCamera()->setPosition( _builder.pos );
    cleft->getCamera()->setQuatAngles( Vector3f{ 0.0f, -M_PI_2, 0.0f } );
    cleft->getCamera()->Mode( CameraMode::Doom );
    cleft->getCamera()->ViewPort( cubemapViewport );
    cleft->getCamera()->enableInputs(false);

    auto cright = addRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Right), cbfb );
    cright->getCamera()->setFoV( cubeMapFOV );
    cright->getCamera()->setPosition( _builder.pos );
    cright->getCamera()->setQuatAngles( Vector3f{ 0.0f, M_PI_2, 0.0f } );
    cright->getCamera()->Mode( CameraMode::Doom );
    cright->getCamera()->ViewPort( cubemapViewport );
    cright->getCamera()->enableInputs(false);

    auto cfront = addRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Front), cbfb );
    cfront->getCamera()->setFoV( cubeMapFOV );
    cfront->getCamera()->setPosition( _builder.pos );
    cfront->getCamera()->setQuatAngles( Vector3f{ 0.0f } );
    cfront->getCamera()->Mode( CameraMode::Doom );
    cfront->getCamera()->ViewPort( cubemapViewport );
    cfront->getCamera()->enableInputs(false);

    auto cback = addRig(  _builder.name + "_" + cubemapFaceToString(CubemapFaces::Back), cbfb );
    cback->getCamera()->setFoV( cubeMapFOV );
    cback->getCamera()->setPosition( _builder.pos );
    cback->getCamera()->setQuatAngles( Vector3f{ 0.0f, M_PI, 0.0f } );
    cback->getCamera()->Mode( CameraMode::Doom );
    cback->getCamera()->ViewPort( cubemapViewport );
    cback->getCamera()->enableInputs(false);
}

std::shared_ptr<CameraRig> CameraManager::addRig( const std::string& _name, const Rect2f& _viewport) {
    auto c = std::make_shared<CameraRig>( _name, _viewport );
    mCameraRigs[_name] = c;
    return c;
}

std::shared_ptr<CameraRig> CameraManager::addRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
    auto c = std::make_shared<CameraRig>( _name, _fb );
    mCameraRigs[_name] = c;
    return c;
}

std::shared_ptr<CameraRig>& CameraManager::getRig( const std::string& _name ) {
    return mCameraRigs[_name];
}

void CameraManager::updateFromInputData( const CameraInputData& _inputData ) {
    for ( auto& [k,v] : mCameraRigs ) {
        v->getMainCamera()->updateFromInputData( _inputData );
    }
}

