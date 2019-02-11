//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <functional>
#include <unordered_map>
#include <core/math/rect2f.h>
#include <render_scene_graph/camera_controls.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <boost/msm/back/state_machine.hpp>
#pragma GCC diagnostic pop

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

struct ImGuiConsole;
class SceneStateMachineBackEnd;
class SceneOrchestrator;

using InitLayoutFunction = std::function<void(SceneStateMachineBackEnd* _layout, SceneOrchestrator*_target)>;
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

class SceneStateMachineBackEnd;

struct SceneStateMachine : msm::front::state_machine_def<SceneStateMachine> {
    explicit SceneStateMachine( SceneStateMachineBackEnd *owner ) : owner( owner ) {}

    struct OnActivate{};

    struct Activate : state<> {};
    struct Run : state<> {};

    void activate( OnActivate const& );

    struct transition_table : mpl::vector<
        a_row<Activate, OnActivate, Run, &SceneStateMachine::activate>
    > {};

    typedef Activate initial_state;

    template <class FSM, class Event> void no_transition(Event const& e, FSM&, int state) {}
private:
    SceneStateMachineBackEnd* owner;
};

class SceneStateMachineBackEnd {
public:
    explicit SceneStateMachineBackEnd( SceneOrchestrator* _p );

    void activate();
    virtual void init() = 0;
    virtual void run() = 0;

    struct Boxes {
        Rect2f& updateAndGetRect();
        Rect2f getRect() const;
        void render( SceneOrchestrator* _target, Rect2f& _rect );
        void toggleVisible();
        void setVisible( bool _bVis );

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
//
//    static std::shared_ptr<SceneStateMachineBackEnd> makeDefault();

    void resizeCallback( SceneOrchestrator* _target, const Vector2i& _resize );

    void render( SceneOrchestrator* _target ) {
        for ( auto& [k,v] : boxes ) {
            v.render( _target, BoxUpdateAndGet(k) );
        }
    }

    InitializeWindowFlagsT getLayoutInitFlags() const {
        return initFlags;
    }

protected:
    void addBoxToViewport( const std::string& _nane, const Boxes& _box );

    SceneOrchestrator* o() const {
        return orchestrator;
    }

private:
    std::unordered_map<std::string, Boxes> boxes;
    InitializeWindowFlagsT initFlags = InitializeWindowFlags::HalfSize;
    SceneOrchestrator* orchestrator = nullptr;
    std::unique_ptr<msm::back::state_machine<SceneStateMachine>> stateMachine;
    std::unordered_map<std::string, std::shared_ptr<LayoutBoxRenderer>> boxFunctionMapping;
    friend class SceneOrchestrator;
};
