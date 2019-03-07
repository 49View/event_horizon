#pragma once

#include <core/camera.h>

class CameraRig;
class Framebuffer;
class Renderer;
struct CameraCubeMapRigBuilder;

class CameraManager {
public:
    std::shared_ptr<CameraRig> addRig( const std::string &_name, const Rect2f& _viewport );
    std::shared_ptr<CameraRig>& getRig( const std::string &_name );

    std::shared_ptr<Camera> getCamera( const std::string &_name = "Main" );
    std::shared_ptr<Camera> getVRLeftEyeCamera( const std::string &_name = "Main" );
    std::shared_ptr<Camera> getVRRightEyeCamera( const std::string &_name = "Main" );
    std::shared_ptr<Camera> getMainCameraRig( const std::string &_name = "Main" );
    std::shared_ptr<Camera> getMainCamera();
    CameraMode getMainCameraMode() const;
    std::string getMainCameraName();
    int CurrEye( const std::string &_name );
    void CurrEye( int val, const std::string &_name );

    void syncCameraMovements( const std::string &camera1, const std::string &camera2 );
    void desyncCameraMovements( const std::string &camera1, const std::string &camera2 );

    void enableInputs( bool _bEnable );
    bool isDoom() const;

    void update();
private:
    std::map<std::string, std::shared_ptr<CameraRig>> mCameraRigs;
    std::vector<std::pair<std::string, std::string>> mSyncedRigs;
};
