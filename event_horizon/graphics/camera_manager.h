#pragma once

#include "camera.h"

class Framebuffer;
class Renderer;

enum class CameraRigType {
    Flat,
    Probe360
};

struct CameraCubeMapRigBuilder {

    CameraCubeMapRigBuilder( const std::string& _name ) {
        name = _name;
    }

    CameraCubeMapRigBuilder& s( int _size ) {
        size = _size;
        return *this;
    }

    CameraCubeMapRigBuilder& at( const Vector3f& _pos ) {
        pos = _pos;
        return *this;
    }

    CameraCubeMapRigBuilder& f( PixelFormat _format ) {
        format = _format;
        return *this;
    }

    CameraCubeMapRigBuilder& useMips() {
        useMipMaps = true;
        return *this;
    }

    std::string name;
    int size = 128;
    Vector3f pos = Vector3f::ZERO;
    PixelFormat format = PIXEL_FORMAT_HDR_RGBA_16;
    bool useMipMaps = false;
};

class CameraRig {
public:
    CameraRig( Renderer& rr, const std::string &_name, const Rect2f& _viewport );
    CameraRig( Renderer& rr, const std::string &_name, std::shared_ptr<Framebuffer> _fb );

    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Camera> getMainCamera() { return mCamera; }
    std::shared_ptr<Camera> getVRLeftCamera() { return mCameraVRLeftEye; }
    std::shared_ptr<Camera> getVRRightCamera() { return mCameraVRRightEye; }

    std::shared_ptr<Framebuffer> getFramebuffer() { return mFramebuffer; };
    void setFramebuffer( std::shared_ptr<Framebuffer> framebuffer );
    const Rect2f& getViewport() const {
        return mViewport;
    }
    void setViewport( const Rect2f& viewport ) {
        mViewport = viewport;
    }

    bool contains( const Vector2f& pos ) const;
    Vector2f mousePickRayOrtho( const Vector2f& _pos );
    bool isActive() const { return mCamera->Status() == CameraState::Active; }

    int CurrEye() const { return mCurrEye; }
    void CurrEye( int val ) { mCurrEye = val; }

    CameraMode getMainCameraMode() const { return mCamera->Mode(); }
    std::string Name() const { return mName; }

protected:
    std::string mName;
    std::shared_ptr<Camera> mCamera;
    std::shared_ptr<Camera> mCameraVRLeftEye;
    std::shared_ptr<Camera> mCameraVRRightEye;

    Rect2f mViewport;
    std::shared_ptr<Framebuffer> mFramebuffer;

    int mCurrEye = 0;
};

class CameraManager {
public:
    std::shared_ptr<CameraRig> addRig( Renderer& rr, const std::string &_name, const Rect2f& _viewport );
    std::shared_ptr<CameraRig> addRig( Renderer& rr, const std::string &_name, std::shared_ptr<Framebuffer> _fb );
    std::shared_ptr<CameraRig>& getRig( const std::string &_name );

    void updateFromInputData( const CameraInputData& mi );
    void addCubeMapRig( Renderer& rr, const CameraCubeMapRigBuilder& _builder );
    std::shared_ptr<Camera> getCamera( const std::string &_name = "Main" ) { return mCameraRigs[_name]->getCamera(); }
    std::shared_ptr<Camera>
    getVRLeftEyeCamera( const std::string &_name = "Main" ) { return mCameraRigs[_name]->getVRLeftCamera(); }
    std::shared_ptr<Camera>
    getVRRightEyeCamera( const std::string &_name = "Main" ) { return mCameraRigs[_name]->getVRRightCamera(); }
    std::shared_ptr<Camera>
    getMainCameraRig( const std::string &_name = "Main" ) { return mCameraRigs[_name]->getMainCamera(); }
    std::shared_ptr<Camera> getMainCamera() { return mCameraRigs["Main"]->getMainCamera(); }
    CameraMode getMainCameraMode() const { return mCameraRigs.find("Main")->second->getMainCameraMode(); }
    std::string getMainCameraName() { return "Main"; }
    int CurrEye( const std::string &_name ) { return mCameraRigs[_name]->CurrEye(); }
    void CurrEye( int val, const std::string &_name ) { mCameraRigs[_name]->CurrEye( val ); }

    void syncCameraMovements( const std::string &camera1, const std::string &camera2 );
    void desyncCameraMovements( const std::string &camera1, const std::string &camera2 );

    void enableInputs( const bool _bEnable );
    bool isDoom() const;

    void update();
private:
    std::map<std::string, std::shared_ptr<CameraRig>> mCameraRigs;
    std::vector<std::pair<std::string, std::string>> mSyncedRigs;
};
