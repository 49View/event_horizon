#pragma once

#include <unordered_map>
#include <core/camera.h>
#include <core/name_policy.hpp>
#include <core/callback_dependency.h>

class CameraRig;
class Renderer;
class SceneGraph;

class CameraManager : public DependencyMakerPolicy<CameraRig> {
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

class CameraBuilder : public NamePolicy<> {
public:
    using NamePolicy::NamePolicy;
    std::shared_ptr<CameraRig> makeDefault( const Rect2f& _viewport, SceneGraph& _md );
};
