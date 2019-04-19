//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <functional>
#include <unordered_map>
#include <core/math/rect2f.h>
#include <core/camera_rig.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <graphics/render_targets.hpp>

struct ImGuiConsole;
class SceneBridge;

using InitLayoutFunction = std::function<void(SceneBridge* _layout, SceneOrchestrator*_target)>;
using RenderFunction = std::function<void( SceneOrchestrator* )>;
using RenderLayoutFunction = std::function<void( SceneOrchestrator* _target, Rect2f& )>;

using PresenterArrangeFunction = std::function<float( float )>;

float sPresenterArrangerLeftFunction( float _value );
float sPresenterArrangerRightFunction( float _value );
float sPresenterArrangerTopFunction( float _value );
float sPresenterArrangerBottomFunction( float _value );

float sPresenterArrangerLeftFunction3d( float _value );
float sPresenterArrangerRightFunction3d( float _value );
float sPresenterArrangerTopFunction3d( float _value );
float sPresenterArrangerBottomFunction3d( float _value );

namespace SceneLayoutDefaultNames {
    const static std::string Taskbar = "taskbar";
    const static std::string Console = "console";
    const static std::string Material = "material";
    const static std::string Scene = "scene";
    const static std::string CloudMaterial = "cloud_material";
    const static std::string CloudGeom = "cloud_geom";
    const static std::string Login = "login";
    const static std::string Timeline = "timeline";
    const static std::string Image = "image";
    const static std::string Camera = "camera";
}

class SceneRectArranger {
public:
    SceneRectArranger() = default;

    explicit SceneRectArranger( const Rect2f& _r ) {
        setRect( _r );
    }

    SceneRectArranger( float leftValue, float rightValue, float topValue, float bottomValue ) : leftValue(
            leftValue ), rightValue( rightValue ), topValue( topValue ), bottomValue( bottomValue ) {}

    SceneRectArranger( const PresenterArrangeFunction& leftFunc, const PresenterArrangeFunction& rightFunc,
                       const PresenterArrangeFunction& topFunc, const PresenterArrangeFunction& bottomFunc,
                       float leftValue, float rightValue, float topValue, float bottomValue ) :
            leftFunc( leftFunc ), rightFunc( rightFunc ), topFunc( topFunc ), bottomFunc( bottomFunc ),
            leftValue(leftValue), rightValue(rightValue), topValue(topValue), bottomValue(bottomValue) {}

    void set() {
        rect.setLeft(leftFunc(leftValue));
        rect.setBottom( bottomFunc(bottomValue) );
        rect.setRight(rightFunc(rightValue));
        rect.setTop( topFunc(topValue) );
        updateScreenPerc();
    }

    void resize();
    void updateScreenPerc();

    const Rect2f& getRect() const {
        return rect;
    }

    Rect2f& getRect() {
        return rect;
    }

    void setRect( const Rect2f& _rect ) {
        SceneRectArranger::rect = _rect;
    }

private:
    PresenterArrangeFunction leftFunc   = sPresenterArrangerLeftFunction;
    PresenterArrangeFunction rightFunc  = sPresenterArrangerRightFunction;
    PresenterArrangeFunction topFunc    = sPresenterArrangerTopFunction;
    PresenterArrangeFunction bottomFunc = sPresenterArrangerBottomFunction;

    float leftValue = 0.0f;
    float rightValue = 1.0f;
    float topValue = 0.0f;
    float bottomValue = 1.0f;

    Rect2f rect = Rect2f::INVALID;
    Rect2f sizeScreenPerc = Rect2f::INVALID;
};

struct SceneScreenBox {
    Rect2f& updateAndGetRect();
    Rect2f getRect() const;
    void render( SceneOrchestrator* _target, Rect2f& _rect );
    void toggleVisible();
    void setVisible( bool _bVis );

    SceneRectArranger rectArranger;
    std::shared_ptr<LayoutBoxRenderer> renderer;
    BoxFlagsT flags = BoxFlags::Rearrange|BoxFlags::Visible;
    static const SceneScreenBox INVALID;
};

class SceneBridge {
public:

//    void render( SceneOrchestrator* _target ) {
//        for ( auto& [k,v] : boxes ) {
//            v.render( _target, BoxUpdateAndGet(k) );
//        }
//    }

private:
    std::unordered_map<std::string, std::shared_ptr<LayoutBoxRenderer>> boxFunctionMapping;
};
