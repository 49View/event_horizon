//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <functional>
#include <unordered_map>
#include <core/math/rect2f.h>
#include <render_scene_graph/camera_controls.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

struct ImGuiConsole;
class SceneLayout;
class Scene;

using DragAndDropFunction = std::function<void(Scene* p, const std::string&)>;
using InitLayoutFunction = std::function<void(SceneLayout* _layout, Scene*_target)>;
using RenderFunction = std::function<void( Scene* )>;
using RenderLayoutFunction = std::function<void( Scene* _target, Rect2f& )>;

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
    const static std::string Geom = "geom";
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

    void resize() {
        rect.percentage( sizeScreenPerc, getScreenSizefUI );
//        updateScreenPerc();
    }

    void updateScreenPerc() {
        sizeScreenPerc = Rect2f::percentage( rect, getScreenRectUI );
    }

    const Rect2f& getRect() const {
        return rect;
    }

    Rect2f& getRect() {
        return rect;
    }

    void setRect( const Rect2f& rect ) {
        SceneRectArranger::rect = rect;
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

class SceneLayout {
public:
    explicit SceneLayout( InitLayoutFunction&& initLayout,
                          RenderFunction&& _renderFunction = nullptr,
                          DragAndDropFunction&& _dd = nullptr,
                          InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal );

    struct Boxes {
        Rect2f& updateAndGetRect() {
            if ( checkBitWiseFlag( flags, BoxFlags::Rearrange ) ) {
                rectArranger.set();
                xandBitWiseFlag( flags, BoxFlags::Rearrange );
            }
            if ( checkBitWiseFlag( flags, BoxFlags::Resize ) ) {
                rectArranger.resize();
                xandBitWiseFlag( flags, BoxFlags::Resize );
            }
            rectArranger.updateScreenPerc();
            return rectArranger.getRect();
        }

        Rect2f getRect() const {
            return rectArranger.getRect();
        }

        void render( Scene* _target, Rect2f& _rect ) {
            if ( renderer ) {
                renderer->render( _target, _rect, flags );
            }
        }

        void toggleVisible() {
            toggle(flags, BoxFlags::Visible);
            if ( renderer ) renderer->toggleVisible();
        }

        void setVisible( bool _bVis ) {
            orBitWiseFlag( flags, BoxFlags::Visible );
            if ( renderer ) renderer->setVisible(_bVis);
        }

        SceneRectArranger rectArranger;
        CameraControls cc = CameraControls::Fly;
        std::shared_ptr<LayoutBoxRenderer> renderer;
        BoxFlagsT flags = BoxFlags::Rearrange|BoxFlags::Visible;
        static const Boxes INVALID;
    };

    void addBox( const std::string& _name, float _l, float _r, float _t, float _b, std::shared_ptr<LayoutBoxRenderer> _lbr );
    void addBox( const std::string& _name, float _l, float _r, float _t, float _b, CameraControls _cc  );
    void addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible = true );

    const Boxes& Box( const std::string& _key ) const {
        if ( const auto& it = boxes.find(_key); it != boxes.end() ) {
            return it->second;
        }
        return Boxes::INVALID;
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

    InitializeWindowFlagsT getInitFlags() const {
        return initFlags;
    }

    void setInitFlags( InitializeWindowFlagsT initFlags ) {
        SceneLayout::initFlags = initFlags;
    }

    static std::shared_ptr<SceneLayout> makeDefault();

    void resizeCallback( Scene* _target, const Vector2i& _resize );

    void render( Scene* _target ) {
        for ( auto& [k,v] : boxes ) {
            v.render( _target, BoxUpdateAndGet(k) );
        }
        if ( renderFunction ) renderFunction( _target );
    }

    void setDragAndDropFunction( DragAndDropFunction dd );

private:
    void activate( Scene* _target );

    std::unordered_map<std::string, Boxes> boxes;
    InitLayoutFunction initLayout;
    RenderFunction renderFunction;
    DragAndDropFunction dragAndDropFunc;
    InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal;

    std::unordered_map<std::string, std::shared_ptr<LayoutBoxRenderer>> boxFunctionMapping;
    Scene* owner = nullptr;
    friend class Scene;
};
