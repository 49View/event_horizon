//
// Created by Dado on 2018-10-29.
//

#pragma once

#include <functional>
#include <unordered_map>
#include <core/math/rect2f.h>
#include <render_scene_graph/camera_controls.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <render_scene_graph/scene_orchestrator.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#pragma GCC diagnostic pop

#define SceneSMBackEndClient(ClassName) \
class ClassName : public SceneStateMachineBackEnd { \
public: \
    using SceneStateMachineBackEnd::SceneStateMachineBackEnd; \
    virtual ~ClassName() = default; \
    void init() override; \
    void run() override;

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;
#define sm_not template <class FSM, class Event> void no_transition(Event const& e, FSM&, int state) {}

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

    sm_not

private:
    SceneStateMachineBackEnd* owner;
};

class SceneOrchestratorDependency {
public:
    SceneOrchestratorDependency( SceneOrchestrator *orchestrator ) : orchestrator( orchestrator ) {}
protected:
    SceneOrchestrator* o() const {
        return orchestrator;
    }
private:
    SceneOrchestrator* orchestrator = nullptr;
};

class SceneStateMachineBackEnd : public SceneOrchestratorDependency {
public:
    explicit SceneStateMachineBackEnd( SceneOrchestrator* _p );

    void activate();
    void initNV();
    virtual void init() = 0;
    virtual void run() = 0;

    struct Boxes {
        Rect2f& updateAndGetRect();
        Rect2f getRect() const;
        void render( SceneOrchestrator* _target, Rect2f& _rect );
        void toggleVisible();
        void setVisible( bool _bVis );

        SceneRectArranger rectArranger;
        std::shared_ptr<LayoutBoxRenderer> renderer;
        BoxFlagsT flags = BoxFlags::Rearrange|BoxFlags::Visible;
        static const Boxes INVALID;
    };

    template <typename T>
    void addRig( const std::string& _name, float _l, float _r, float _t, float _b ) {
        Boxes _box{ { sPresenterArrangerLeftFunction3d,
                      sPresenterArrangerRightFunction3d,
                      sPresenterArrangerTopFunction3d,
                      sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, nullptr };
        addBoxToViewport( _name, _box );
        auto lViewport = boxes[_name].updateAndGetRect();
        o()->addViewport<T>( RenderTargetType::PBR, _name, lViewport, BlitType::OnScreen );
    }

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

    void resizeCallback( SceneOrchestrator* _target, const Vector2i& _resize );

    void render( SceneOrchestrator* _target ) {
        for ( auto& [k,v] : boxes ) {
            v.render( _target, BoxUpdateAndGet(k) );
        }
    }

protected:
    void postDefaults();
    void addBoxToViewport( const std::string& _nane, const Boxes& _box );

private:
    std::unordered_map<std::string, Boxes> boxes;
    std::unique_ptr<msm::back::state_machine<SceneStateMachine>> stateMachine;
    std::unordered_map<std::string, std::shared_ptr<LayoutBoxRenderer>> boxFunctionMapping;
    friend class SceneOrchestrator;
};
