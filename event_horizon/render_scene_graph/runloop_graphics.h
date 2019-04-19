//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include <core/runloop_core.h>
#include <core/util.h>
#include <core/http/webclient.h>
#include <core/di.hpp>
#include <core/state_machine_helper.hpp>
#include <core/command.hpp>
#include <graphics/window_handling.hpp>
#include <core/math/anim.h>
#include <poly/scene_graph.h>
#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/render_scene_graph.h>

namespace di = boost::di;


template <typename FE>
class RunLoopBackEnd {
public:
    using BE = msm::back::state_machine<FE>;

    RunLoopBackEnd( SceneGraph& _sg, RenderSceneGraph& _rsg) : sg(_sg), rsg(_rsg) {
        backEnd = std::make_unique<BE>( &_sg, &_rsg );
    }
    virtual ~RunLoopBackEnd() = default;

    void update( const AggregatedInputData& _aid ) {
        rsg.updateInputs( _aid );
    };

    void activate() {
        sg.init();
        rsg.init();
        backEnd->start();
    }

protected:
    SceneGraph& sg;
    RenderSceneGraph& rsg;
    std::unique_ptr<BE> backEnd;
};

template <typename FE>
class RunLoopGraphics : public RunLoop {
public:
    using RunLoop::RunLoop;
	RunLoopGraphics( CommandQueue& _cq, Renderer& rr, TextInput& ti,
	                 MouseInput& mi, SceneGraph& _sg, RenderSceneGraph& _rsg )
                     : RunLoop( _cq ), rr( rr ), ti( ti), mi( mi), sg(_sg), rsg(_rsg) {
        rlbackEnd = std::make_unique<RunLoopBackEnd<FE>>( sg, rsg );
	}

    void init( InitializeWindowFlagsT _initFlags ) {
        WH::initializeWindow( _initFlags, rr );
        rr.init();
        //	mi.subscribe( pm );
    }

    void run(){
        // This will be use for multithreading rendering
        // to be supported properly within wasm, atm out of the scope of current understanding :D
    }

    void singleThreadLoop() {

        update();
        render();

        nTicks++;
    }

    void runSingleThread() override {
        rlbackEnd->activate();
        while ( !WH::shouldWindowBeClosed() ) {
            singleThreadLoop();
        }
        Http::shutDown();
    }


    void addScriptLine( const std::string& _cmd ) {
        cq.script( _cmd );
    }

    Renderer& RR() { return rr; }
    TextInput& TI() { return ti; }
	MouseInput& MI() { return mi; }

protected:
	void update() {
        updateTime();
        WH::preUpdate();
        mUpdateSignals.NeedsUpdate(false);
        Timeline::update();
        cq.execute();
        WH::pollEvents();
        mi.update( mUpdateSignals );
        rlbackEnd->update(aggregateInputs());
    }

    void render() {
//        ImGui::NewFrame();
//        stateMachine->render( this );
//        for ( auto& [k,v] : StateMachine()->getRigs() ) {
//            v->renderControls(this);
//        }
        RR().directRenderLoop();
//        ImGui::Render();

        WH::flush();
    }

    auto aggregateInputs() {
        return AggregatedInputData{ ti,
                             mi.getCurrPos(),
                             mi.isTouchedDown(),
                             mi.isTouchedDownFirstTime(),
                             false,//notifications.singleMouseTapEvent,
                             mi.getScrollValue(),
                             mi.getCurrMoveDiff( YGestureInvert::No ).dominant()*0.01f,
                             mi.getCurrMoveDiffNorm().dominant() };

    }

protected:
    Renderer& rr;
    TextInput& ti;
    MouseInput& mi;
    SceneGraph& sg;
    RenderSceneGraph& rsg;

	int nUpdates = 0;
	int nRenders = 0;
	int nTicks = 0;
	UpdateSignals mUpdateSignals;
	std::unique_ptr<RunLoopBackEnd<FE>> rlbackEnd;
};

template <typename FE>
void mainLoop( InitializeWindowFlagsT initFlags ) {
    auto rl = di::make_injector().create<RunLoopGraphics<FE>>();
    rl.init( initFlags );
    rl.runSingleThread();
}
