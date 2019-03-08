//
//  SceneOrchestrator
//
//

#include "scene_orchestrator.hpp"

#include "core/camera_manager.h"
#include "graphics/renderer.h"
#include "graphics/render_list.h"
#include "core/camera_rig.hpp"
#include "graphics/window_handling.hpp"
#include "graphics/ui/imgui_console.h"

#include <render_scene_graph/scene_state_machine.h>
#include <render_scene_graph/render_scene_graph.h>

#include <stb/stb_image.h>

#include "di_modules.h"

std::vector<std::string> SceneOrchestrator::callbackPaths;
Vector2i SceneOrchestrator::callbackResizeWindow = Vector2i(-1, -1);
Vector2i SceneOrchestrator::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> SceneOrchestrator::sUpdateCallbacks;

class CommandScriptPresenterManager : public CommandScript {
public:
	explicit CommandScriptPresenterManager( SceneOrchestrator& hm );
	virtual ~CommandScriptPresenterManager() = default;
};

CommandScriptPresenterManager::CommandScriptPresenterManager( SceneOrchestrator& _hm ) {
    addCommandDefinition("enable keyboard", std::bind(&SceneOrchestrator::cmdEnableKeyboard, &_hm, std::placeholders::_1));
    addCommandDefinition("disable keyboard", std::bind(&SceneOrchestrator::cmdDisableKeyboard, &_hm, std::placeholders::_1));
	addCommandDefinition("change time", std::bind(&SceneOrchestrator::cmdChangeTime, &_hm, std::placeholders::_1 ));
}

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
	ASSERT( count > 0 );

	SceneOrchestrator::callbackPaths.clear();
	for ( auto i = 0; i < count; i++ ) {
		SceneOrchestrator::callbackPaths.emplace_back( std::string(paths[i]) );
	}
}

void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	SceneOrchestrator::callbackResizeWindow = Vector2i{w, h};
}

void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	SceneOrchestrator::callbackResizeFrameBuffer = Vector2i{w, h};
}

SceneOrchestrator::SceneOrchestrator( Renderer& _rr, RenderSceneGraph& _rsg, FontManager& _fm, TextInput& ti, MouseInput& mi, CameraManager& cm, CommandQueue& cq, StreamingMediator& _ssm ) :
		cm(cm), rr(_rr), rsg(_rsg), fm( _fm ), ti( ti), mi( mi ), cq( cq), ssm(_ssm) {
	hcs = std::make_shared<CommandScriptPresenterManager>(*this);
	cq.registerCommandScript(hcs);
	console = std::make_shared<ImGuiConsole>(cq);
}

void SceneOrchestrator::setDragAndDropFunction( DragAndDropFunction dd ) {
	dragAndDropFunc = dd;
}

void SceneOrchestrator::updateCallbacks() {

	if ( !sUpdateCallbacks.empty() ) {
		for ( auto& c : sUpdateCallbacks ) {
			c( this );
		}
		sUpdateCallbacks.clear();
	}

	if ( !callbackPaths.empty() ) {
		for ( auto& path : callbackPaths ) {
			if ( dragAndDropFunc ) dragAndDropFunc( this, path );
		}
		callbackPaths.clear();
	}

	if ( callbackResizeWindow.x() > 0 && callbackResizeWindow.y() > 0 ) {
		// For now we do everything in the callbackResizeFrameBuffer so this is redundant for now, just a nop
		// to be re-enabled in the future if we need it
//		LOGR("Resized window: [%d, %d]", callbackResizeWindow.x(), callbackResizeWindow.y() );
		callbackResizeWindow = Vector2i{-1, -1};
	}

	if ( callbackResizeFrameBuffer.x() > 0 && callbackResizeFrameBuffer.y() > 0 ) {
		WH::resizeWindow( callbackResizeFrameBuffer );
		WH::gatherMainScreenInfo();
		RR().resetDefaultFB(callbackResizeFrameBuffer);
		stateMachine->resizeCallback( this, callbackResizeFrameBuffer );
		callbackResizeFrameBuffer = Vector2i{-1, -1};
	}

}

void SceneOrchestrator::update() {
	stateMachine->run();
	inputPollUpdate();
	updateCallbacks();
	RSG().update();
}

void SceneOrchestrator::deactivate() {
}

void SceneOrchestrator::activate() {

	WH::setDropCallback( GDropCallback );
	WH::setResizeWindowCallback( GResizeWindowCallback );
	WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

    //stbi_set_flip_vertically_on_load(true);

#ifndef _PRODUCTION_
	Socket::on( "shaderchange",
				std::bind(&SceneOrchestrator::reloadShaders, this, std::placeholders::_1 ) );
#endif

	ImageBuilder{S::WHITE}.makeDirect( rsg.TL(), RawImage::WHITE4x4() );
	ImageBuilder{S::BLACK}.makeDirect( rsg.TL(), RawImage::BLACK_RGBA4x4 );
	ImageBuilder{S::NORMAL}.makeDirect( rsg.TL(), RawImage::NORMAL4x4 );
	ImageBuilder{S::DEBUG_UV}.makeDirect( rsg.TL(), RawImage::DEBUG_UV() );

	MaterialBuilder{S::WHITE_PBR, S::SH}.makeDefault(rsg.ML());

	stateMachine->activate();
}

void SceneOrchestrator::reloadShaders( SocketCallbackDataType _data ) {

	ShaderLiveUpdateMap shadersToUpdate{_data};

	for ( const auto& ss : shadersToUpdate.shaders ) {
		rr.injectShader( ss.first, ss.second );
	}
	cq.script( "reload shaders" );
}

void SceneOrchestrator::enableInputs( bool _bEnabled ) {
	ti.setEnabled( _bEnabled );
}

void SceneOrchestrator::resetSingleEventNotifications() {
    notifications.singleMouseTapEvent = false;
}

void SceneOrchestrator::inputPollUpdate() {

	CameraInputData cid{ ti,
			  mi.getCurrPos(),
			  mi.isTouchedDown(),
			  mi.isTouchedDownFirstTime(),
			  notifications.singleMouseTapEvent,
			  mi.getScrollValue(),
			  mi.getCurrMoveDiff( YGestureInvert::No ).dominant()*0.01f,
			  mi.getCurrMoveDiffNorm().dominant() };

	for ( auto& [k,v] : mRigs ) {
		v->updateFromInputData( cid );
	}

	cm.update();

	resetSingleEventNotifications();
}

bool SceneOrchestrator::checkKeyPressed( int keyCode ) {
	return ti.checkKeyPressed( keyCode );
}

bool SceneOrchestrator::checkKeyToggled( int keyCode ) {
	return ti.checkKeyToggleOn( keyCode );
}

void SceneOrchestrator::notified( MouseInput& _source, const std::string& generator ) {

	if ( generator == "onTouchUp" ) {
//		onTouchUpImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else if ( generator == "onSingleTap" ) {
		notifications.singleMouseTapEvent = true;
	} else if ( generator == "onDoubleTap" ) {
//		onDoubleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	}
	//LOGR( generator.c_str() );
}

void SceneOrchestrator::render() {
	ImGui::NewFrame();

	stateMachine->render( this );

	for ( auto& [k,v] : mRigs ) {
		v->renderControls(this);
	}

	rr.directRenderLoop( Targets() );

	ImGui::Render();
}

void SceneOrchestrator::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
	sUpdateCallbacks.push_back( uc );
}

const std::string SceneOrchestrator::DC() {
	return Name::Foxtrot;
}

void SceneOrchestrator::cmdEnableKeyboard( const std::vector<std::string>& params ) {
    WH::enableInputCallbacks();
}

void SceneOrchestrator::cmdDisableKeyboard( const std::vector<std::string>& params ) {
    WH::disableInputCallbacks();
}

void SceneOrchestrator::cmdChangeTime( const std::vector<std::string>& _params ) {
	RSG().SB().buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
	changeTime( RSG().SB().getSunPosition() );
}

void SceneOrchestrator::StateMachine( std::shared_ptr<SceneStateMachineBackEnd> _l ) {
	stateMachine = _l;
}

const std::shared_ptr<ImGuiConsole>& SceneOrchestrator::Console() const {
	return console;
}

void SceneOrchestrator::takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata ) {
    addViewport<RLTargetPBR>( Name::Sierra, Rect2f( Vector2f::ZERO, Vector2f{128.0f} ), CameraControls::Fly,
    		                  BlitType::OffScreen );
    getCamera(Name::Sierra)->center(_box);
    getTarget(Name::Sierra)->takeScreenShot( _outdata );
}

void SceneOrchestrator::clearTargets() {
	for ( const auto& target : mTargets ) {
		target->clearCB();
	}
}

std::shared_ptr<RLTarget> SceneOrchestrator::getTarget( const std::string& _name ) {

	for ( auto& t : mTargets ) {
		if ( t->cameraRig->Name() == _name ) return t;
	}

	return nullptr;
}

void SceneOrchestrator::changeTime( const V3f& _solar ) {
	for ( auto& t : mTargets ) {
		t->changeTime( _solar );
	}
}

RenderSceneGraph& SceneOrchestrator::RSG() { return rsg; }
MaterialManager& SceneOrchestrator::ML() { return rsg.ML(); }
Renderer& SceneOrchestrator::RR() { return rr; }
CameraManager& SceneOrchestrator::CM() { return cm; }
TextureManager& SceneOrchestrator::TM() { return rr.TM(); }
CommandQueue& SceneOrchestrator::CQ() { return cq; }
FontManager& SceneOrchestrator::FM() { return fm; }
StreamingMediator& SceneOrchestrator::SSM() { return ssm; }

std::shared_ptr<Camera> SceneOrchestrator::getCamera( const std::string& _name ) { return CM().getCamera(_name); }

const cameraRigsMap& SceneOrchestrator::getRigs() const {
	return mRigs;
}

void SceneOrchestrator::script( const std::string& _commandLine ) {
	CQ().script( _commandLine );
}

InitializeWindowFlagsT SceneOrchestrator::getLayoutInitFlags() const {
	return stateMachine->getLayoutInitFlags();
}

std::shared_ptr<SceneStateMachineBackEnd> SceneOrchestrator::StateMachine()  {
    return stateMachine;
}




