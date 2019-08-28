//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <string>
#include <core/observer.h>
#include <core/soa_utils.h>
#include <core/streaming_mediator.hpp>
#include <poly/scene_graph.h>
#include <graphics/renderer.h>
#include <graphics/render_targets.hpp>
#include <render_scene_graph/camera_controls.hpp>
#include <render_scene_graph/scene_bridge.h>
#define SOL_ALL_SAFETIES_ON 1
#include <lua/sol/sol.hpp> // or #include "sol.hpp", whichever suits your needs

struct scene_t;
struct PickRayData;
class Renderer;
class VData;
struct AggregatedInputData;

using cameraRigsMap = std::unordered_map<std::string, std::shared_ptr<CameraControl>>;
using UpdateCallbackSign = RenderOrchestrator*;
using PresenterUpdateCallbackFunc = std::function<void(UpdateCallbackSign)>;
using DragAndDropFunction = std::function<void(std::vector<std::string>& _paths)>;

class RenderOrchestrator {
public:
    RenderOrchestrator( Renderer& rr, SceneGraph& _sg );
    virtual ~RenderOrchestrator() = default;

    void init();
    void updateInputs( const AggregatedInputData& _aid );

    // Viewport madness
    template <typename T>
    void addRig( const std::string& _name, float _l, float _r, float _t, float _b ) {
        SceneScreenBox _box{ { sPresenterArrangerLeftFunction3d,
                               sPresenterArrangerRightFunction3d,
                               sPresenterArrangerTopFunction3d,
                               sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, nullptr };
        addBoxToViewport( _name, _box );
        auto lViewport = boxes[_name].updateAndGetRect();
        addViewport<T>( RenderTargetType::PBR, _name, lViewport, BlitType::OffScreen );
    }

    template <typename T>
    void addViewport( RenderTargetType _rtt, const std::string& _rigname, const Rect2f& _viewport, BlitType _bt ) {
        auto _rig = getRig(_rigname);
        _rig->setViewport(_viewport);

        if ( mRigs.find(_rig->Name()) == mRigs.end() ) {
            RenderTargetFactory::make( _rtt, _rig, _viewport, _bt, rr );
            mRigs[_rig->Name()] = std::make_shared<T>( _rig, *this );
        } else {
            setViewportOnRig( _rig, _viewport );
        }
    }

    template <typename T>
    void setRigCameraController( const std::string& _rigname = Name::Foxtrot ) {
        if ( auto rig = getRig(_rigname); rig ) {
            mRigs[rig->Name()] = std::make_shared<T>( rig, *this );
        }
    }

    std::shared_ptr<Camera> DC() {
        return getRig( Name::Foxtrot )->getCamera();
    }

    void addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible = true );

    const SceneScreenBox& Box( const std::string& _key ) const {
        if ( const auto& it = boxes.find(_key); it != boxes.end() ) {
            return it->second;
        }
        return SceneScreenBox::INVALID;
    }

    Rect2f& BoxUpdateAndGet( const std::string& _key ) {
        if ( auto it = boxes.find(_key); it != boxes.end() ) {
            return it->second.updateAndGetRect();
        }
        static Rect2f invalid{Rect2f::INVALID};
        return invalid;
    }

    void toggleVisible( const std::string& _key ) {
        if ( auto it = boxes.find(_key); it != boxes.end() ) {
            it->second.toggleVisible();
        }
    }

    void resizeCallback( const Vector2i& _resize );

    PickRayData rayViewportPickIntersection( const V2f& _screenPos ) const;
    std::shared_ptr<CameraRig> getRig( const std::string& _name );

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );
    void setDragAndDropFunction( DragAndDropFunction dd );
    void reloadShaders( const std::string& _msg, SocketCallbackDataType&& _data );

    template <typename T>
    void addHttpStream( const std::string& _streamName ) {
        rr.SSM().addStream<T>( _streamName, avcbTM() );
    }

    void clearPBRRender( const std::string& _target = "" );
    void hidePBRRender( const std::string& _target = "" );
    void showPBRRender( const std::string& _target = "" );
    void setVisible( uint64_t _cbIndex, bool _value );
    void createSkybox( const SkyBoxInitParams& _skyboxParams );
    void useSkybox( bool _value );
    void useSSAO( bool _value );
    void changeTime( const std::string& _time );
    floata& skyBoxDeltaInterpolation();

    const std::string& getLuaScriptHotReload() const {
        return luaScriptHotReload;
    }

    void setLuaScriptHotReload( const std::string& _luaScriptHotReload ) {
        luaScriptHotReload = _luaScriptHotReload;
    }

protected:
    AVInitCallback avcbTM();
    std::shared_ptr<Camera>    getCamera( const std::string& _name );
    const Camera* getCamera( const std::string& _name ) const;
    void addBoxToViewport( const std::string& _nane, const SceneScreenBox& _box );
    void setViewportOnRig( std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport );
    void setViewportOnRig( const std::string& _rigName, const Rect2f& _viewport );
    void luaUpdate(const AggregatedInputData& _aid);

public:
    Renderer& RR();
    SceneGraph& SG() { return sg; }
protected:
    void updateCallbacks();
    void resizeCallbacks();
    void initWHCallbacks();
    void changeMaterialTagCallback( const std::vector<std::string>& _params );
    void changeMaterialColorCallback( const std::vector<std::string>& _params );

    int bake(scene_t *scene);

private:
    Renderer& rr;
    SceneGraph& sg;
    cameraRigsMap mRigs;
    std::unordered_map<std::string, SceneScreenBox> boxes;
    sol::state lua{};
    std::string luaScriptHotReload;

    DragAndDropFunction dragAndDropFunc = nullptr;

public:
    static std::vector<std::string> callbackPaths;
    static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;
};



