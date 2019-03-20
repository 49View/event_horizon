#pragma once

#include <unordered_map>
#include <core/camera.h>
#include <poly/resources/builders.hpp>

class CameraRig;
class Renderer;
class SceneGraph;

class CameraManager : public ResourceManager<CameraRig> {
public:
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
    std::vector<std::pair<std::string, std::string>> mSyncedRigs;
};

class CameraBuilder : public ResourceBuilder<CameraRig, CameraManager> {
    using ResourceBuilder::ResourceBuilder;
public:
    void makeDefault() override;
};
