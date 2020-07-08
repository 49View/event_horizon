//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include <core/runloop_core.h>
#include <core/http/webclient.h>
#include <core/di.hpp>
#include <core/cli_param_map.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/state_machine_helper.hpp>
#include <graphics/window_handling.hpp>
#include <poly/scene_graph.h>
#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/render_orchestrator.h>

namespace di = boost::di;

class RunLoopBackEndBase {
public:
    RunLoopBackEndBase( SceneGraph& _sg, RenderOrchestrator& _rsg) : sg(_sg), rsg(_rsg) {
    }
    virtual ~RunLoopBackEndBase() = default;

    const CLIParamMap &getCLIParams() const {
        return cliParams;
    }

    void setCLIParams( const CLIParamMap &params ) {
        cliParams = params;
    }

    void update( AggregatedInputData& _aid, MouseInput& mi ) {
        mi.UseCaptureOnMove(rsg.getMICursorCapture());
        rsg.updateInputs( _aid );
        sg.update();
        updateImpl( _aid );
    };

    void activate(const CLIParamMap& params) {
        sg.init();
        rsg.init(params);
        luaFunctionsSetup();
        activateImpl();
    }

    virtual void updateImpl( const AggregatedInputData& _aid ) = 0;
    virtual void activateImpl() = 0;
    virtual void luaFunctionsSetup() {}

protected:
    CLIParamMap cliParams;
    SceneGraph& sg;
    RenderOrchestrator& rsg;
};

[[nodiscard]] std::optional<InitializeWindowFlagsT> checkLayoutParam( const std::string& _param );
InitializeWindowFlagsT checkLayoutArgvs( const CLIParamMap& params );

class RunLoopGraphics : public RunLoop {
public:
    using RunLoop::RunLoop;
	RunLoopGraphics( Renderer& rr, TextInput& ti,
	                 MouseInput& mi, SceneGraph& _sg, RenderOrchestrator& _rsg )
                     : RunLoop(), rr( rr ), ti( ti), mi( mi), sg(_sg), rsg(_rsg) {}

    void setBackEnd( std::unique_ptr<RunLoopBackEndBase>&& _be ) {
        rlbackEnd = std::move(_be);
	}

    void init( const CLIParamMap& params ) {
        InitializeWindowFlagsT initFlags = checkLayoutArgvs( params );
        WH::initializeWindow( initFlags, rr );
        rr.init();
        rlbackEnd->activate(params);
        //	mi.subscribe( pm );
    }

    void run(){
        // This will be use for multithreading rendering
        // to be supported properly within wasm, atm out of the scope of current understanding :D
    }

    void updateLuaScript( const std::string& _lscript ) {
	    rsg.setLuaScriptHotReload( _lscript );
	}

	void reloadShadersViaHttp() {
	    rsg.reloadShadersViaHttp();
	}

    void singleThreadLoop() {

        update();
        render();

        nTicks++;
    }

    void runSingleThread() override {
        while ( !WH::shouldWindowBeClosed() ) {
            singleThreadLoop();
        }
        Http::shutDown();
    }

    Renderer& RR() { return rr; }
    TextInput& TI() { return ti; }
	MouseInput& MI() { return mi; }

protected:
	void update() {
        updateTime();
        mUpdateSignals.NeedsUpdate(false);
        Timeline::update();
        WH::pollEvents();
        WH::preUpdate();
        mi.update( mUpdateSignals );
        auto aid = aggregateInputs();
        rlbackEnd->update( aid, mi );
    }

    void render() {
        RenderStats rs{};
        RR().directRenderLoop(rs);
        WH::imRenderLoopStats(rs);
        WH::flush();
    }

    AggregatedInputData aggregateInputs() {
	    ti.setEnabled(WH::isKeyboardInputActive());
        return AggregatedInputData{ ti, mi.getScrollValue(), mi.Status() };
    }

protected:
    Renderer& rr;
    TextInput& ti;
    MouseInput& mi;
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    std::unique_ptr<RunLoopBackEndBase> rlbackEnd;

	int nUpdates = 0;
	int nRenders = 0;
	int nTicks = 0;
	UpdateSignals mUpdateSignals;
};

