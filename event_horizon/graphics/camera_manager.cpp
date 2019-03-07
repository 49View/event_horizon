#include "camera_manager.h"

#include "core/service_factory.h"
#include "core/app_globals.h"
#include "graphics/graphic_functions.hpp"
#include "graphics/framebuffer.h"
#include "graphics/camera_rig.hpp"
//#include "vr_manager.hpp"

void CameraManager::syncCameraMovements( const std::string& cameraSource, const std::string& cameraDest ) {
    mSyncedRigs.emplace_back( cameraSource, cameraDest );
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
        getCamera( c.second )->setQuat( getCamera( c.first )->quatAngle());
        getCamera( c.second )->setFoV( getCamera( c.first )->FoV());
        getCamera( c.second )->ViewPort( getCamera( c.first )->ViewPort());
    }

    for ( auto& c : mCameraRigs ) {
        c.second->getMainCamera()->update();
        // Copy camera values to left and right camera for VR
        c.second->getVRLeftCamera()->setPosition( c.second->getMainCamera()->getPosition());
        c.second->getVRLeftCamera()->setQuat( c.second->getMainCamera()->quatAngle());
        c.second->getVRLeftCamera()->setFoV( c.second->getMainCamera()->FoV());
        c.second->getVRLeftCamera()->ViewPort( c.second->getMainCamera()->ViewPort());
        c.second->getVRRightCamera()->setPosition( c.second->getMainCamera()->getPosition());
        c.second->getVRRightCamera()->setQuat( c.second->getMainCamera()->quatAngle());
        c.second->getVRRightCamera()->setFoV( c.second->getMainCamera()->FoV());
        c.second->getVRRightCamera()->ViewPort( c.second->getMainCamera()->ViewPort());

//        VRM.preRenderEye( *this, 0, c.first );
//        VRM.preRenderEye( *this, 1, c.first );
    }
}

std::shared_ptr<CameraRig> CameraManager::addRig( const std::string& _name, const Rect2f& _viewport) {
    auto c = std::make_shared<CameraRig>( _name, _viewport );
    mCameraRigs[_name] = c;
    return c;
}

//std::shared_ptr<CameraRig> CameraManager::addRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
//    auto c = std::make_shared<CameraRig>( _name, _fb );
//    mCameraRigs[_name] = c;
//    return c;
//}

std::shared_ptr<CameraRig>& CameraManager::getRig( const std::string& _name ) {
    return mCameraRigs[_name];
}

std::shared_ptr<Camera> CameraManager::getCamera( const std::string& _name ) { return mCameraRigs[_name]->getCamera(); }

std::shared_ptr<Camera> CameraManager::getVRLeftEyeCamera( const std::string& _name ) { return mCameraRigs[_name]->getVRLeftCamera(); }

std::shared_ptr<Camera> CameraManager::getVRRightEyeCamera( const std::string& _name ) { return mCameraRigs[_name]->getVRRightCamera(); }

std::shared_ptr<Camera> CameraManager::getMainCameraRig( const std::string& _name ) { return mCameraRigs[_name]->getMainCamera(); }

std::shared_ptr<Camera> CameraManager::getMainCamera() { return mCameraRigs["Main"]->getMainCamera(); }

CameraMode CameraManager::getMainCameraMode() const { return mCameraRigs.find("Main")->second->getMainCameraMode(); }

std::string CameraManager::getMainCameraName() { return "Main"; }

int CameraManager::CurrEye( const std::string& _name ) { return mCameraRigs[_name]->CurrEye(); }

void CameraManager::CurrEye( int val, const std::string& _name ) { mCameraRigs[_name]->CurrEye( val ); }

