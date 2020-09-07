

//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <string>
#include <core/observer.h>
#include <core/streaming_mediator.hpp>
#include <render_scene_graph/camera_controls.hpp>
#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/ui_view.hpp>
#include <core/file_watcher.hpp>

#define SOL_ALL_SAFETIES_ON 1

#include <lua/sol/sol.hpp> // or #include "sol.hpp", whichever suits your needs
#include <core/state_machine_helper.hpp>

struct scene_t;
struct PickRayData;
struct AggregatedInputData;
struct ShaderLiveUpdateMap;

class Renderer;

class SceneGraph;

class VData;

class UIView;

class CLIParamMap;

enum class UIElementStatus;

using cameraRigsMap = std::unordered_map<std::string, std::shared_ptr<CameraControl>>;
using UpdateCallbackSign = RenderOrchestrator *;
using PresenterUpdateCallbackFunc = std::function<void( UpdateCallbackSign )>;
using DragAndDropFunction = std::function<void( std::vector<std::string>& _paths )>;

class RenderOrchestrator {
public:
    RenderOrchestrator( Renderer& rr, SceneGraph& _sg );
    virtual ~RenderOrchestrator() = default;

    void init( const CLIParamMap& params );
    void updateInputs( AggregatedInputData& _aid );

    // Viewport madness
    void addRig( CameraControlType _ct, const std::string& _name, float _l, float _r, float _t, float _b );
    void
    addViewport( CameraControlType _ct, RenderTargetType _rtt, const std::string& _rigname, const Rect2f& _viewport,
                 BlitType _bt );
    void setRigCameraController( CameraControlType _ct, const std::string& _rigname = Name::Foxtrot );
    CameraControlType getRigCameraController( const std::string& _rigname = Name::Foxtrot );

    std::shared_ptr<Camera> DC();

    void addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible = true );

    const SceneScreenBox& Box( const std::string& _key ) const;

    [[maybe_unused]] [[maybe_unused]] Rect2f& BoxUpdateAndGet( const std::string& _key );

    void toggleVisible( const std::string& _key );

    void clearUIView();
    void
    addUIContainer( const MPos2d& _at, CResourceRef _res, UIElementStatus _initialStatus );

    void resizeCallback( const Vector2i& _resize );

    PickRayData rayViewportPickIntersection( const V2f& _screenPos ) const;
    std::shared_ptr<CameraRig> getRig( const std::string& _name );

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );
    void setDragAndDropFunction( DragAndDropFunction dd );
    void reloadShaders( const ShaderLiveUpdateMap& shadersToUpdate );
    void reloadShaders( const std::string& );

    template<typename T>
    void addHttpStream( const std::string& _streamName ) {
        // ### NDDado: check that it will still work
//        rr.SSM().addStream<T>(_streamName, avcbTM());
    }

    void setDirtyFlagOnPBRRender( const std::string& _target, const std::string& _sub, bool _flag );
    void clearPBRRender( const std::string& _target = "" );
    void hidePBRRender( const std::string& _target = "" );
    void showPBRRender( const std::string& _target = "" );
    void setVisible( uint64_t _cbIndex, bool _value );
    void setProbePosition( const V3f& _pos );
    void setSkyboxCenter( const V3f& _value );
    void createSkybox( const SkyBoxInitParams& _skyboxParams );
    void useSkybox( bool _value );
    void useSunLighting( bool _value );
    void useSSAO( bool _value );
    void useDOF( bool _value );
    void useMotionBlur( bool _value );
    void takeScreenShot( std::function<void( const SerializableContainer& )> screenShotCallback );
    void changeTime( const std::string& _time, float _artificialWorldRotationAngle );
    void addSecondsToTime( int _seconds, float _artificialWorldRotationAngle );
    void changeCameraControlType( int _type );

    floata& skyBoxDeltaInterpolation();

    template<typename T>
    void addLuaFunction( const std::string& _key, const std::string& _fname, T _func ) {
        auto luaKey = lua[_key].get_or_create<sol::table>();
        luaKey[_fname] = _func;
    }

    const std::string& getLuaScriptHotReload() const;
    void setLuaScriptHotReload( const std::string& _luaScriptHotReload );

    void reloadShadersViaHttp();
    void setMICursorCapture( bool _flag, MouseCursorType _mct = MouseCursorType::ARROW );
    [[nodiscard]] std::pair<bool, MouseCursorType> getMICursorCapture() const;

protected:
    AVInitCallback avcbTM();
    std::shared_ptr<Camera> getCamera( const std::string& _name );
    const Camera *getCamera( const std::string& _name ) const;
    void addBoxToViewport( const std::string& _nane, const SceneScreenBox& _box );
    void setViewportOnRig( std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport );
    void setViewportOnRig( const std::string& _rigName, const Rect2f& _viewport );
    void luaUpdate( const AggregatedInputData& _aid );

public:
    Renderer& RR();
    SceneGraph& SG();
    UIView& UI();
    UICallbackMap& UICB();
    unsigned int TH( CResourceRef _value ); // direct access to texture handle, it's a very common pattern for GUIs so let's do it
protected:
    void uiViewUpdate( AggregatedInputData& _aid );
    void updateCallbacks();
    void resizeCallbacks();
    void initWHCallbacks();
    void changeMaterialTagCallback( const std::vector<std::string>& _params );
    void changeMaterialColorCallback( const std::vector<std::string>& _params );

    int bake( scene_t *scene );

private:
    Renderer& rr;
    SceneGraph& sg;
    UIView uiView;
    cameraRigsMap mRigs;
    bool bMICursorCapture = false;
    MouseCursorType currMouseCursorType = MouseCursorType::ARROW;
    std::unordered_map<std::string, SceneScreenBox> boxes;
    sol::state lua{};
    std::string luaScriptHotReload;
#ifndef _PRODUCTION_
    std::unique_ptr<FileWatcher> fw;
#endif
    DragAndDropFunction dragAndDropFunc = nullptr;

public:
    static std::vector<std::string> callbackPaths;
    static std::vector<std::string>& CallbackPaths();
    static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;
};



