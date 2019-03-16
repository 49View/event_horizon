//
//  SceneOrchestrator
//
//

#include "scene_orchestrator.hpp"

#include <poly/camera_manager.h>
#include <core/camera_rig.hpp>
#include <graphics/renderer.h>
#include <graphics/render_list.h>
#include <graphics/shader_manager.h>
#include <graphics/window_handling.hpp>
#include <graphics/ui/imgui_console.h>
#include <graphics/render_targets.hpp>

#include <render_scene_graph/scene_state_machine.h>
#include <render_scene_graph/render_scene_graph.h>

#include <stb/stb_image.h>

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
//    addCommandDefinition("enable keyboard", std::bind(&SceneOrchestrator::cmdEnableKeyboard, &_hm, std::placeholders::_1));
//    addCommandDefinition("disable keyboard", std::bind(&SceneOrchestrator::cmdDisableKeyboard, &_hm, std::placeholders::_1));
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

SceneOrchestrator::SceneOrchestrator( SceneGraph& _sg,
                                      RenderSceneGraph& _rsg,
									  TextInput& ti,
									  MouseInput& mi,
									  CommandQueue& cq ) :
									  sg(_sg), rsg(_rsg), ti( ti), mi( mi ), cq( cq) {
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
		RSG().RR().resetDefaultFB(callbackResizeFrameBuffer);
		stateMachine->resizeCallback( this, callbackResizeFrameBuffer );
		callbackResizeFrameBuffer = Vector2i{-1, -1};
	}

}

void SceneOrchestrator::update() {
	stateMachine->run();
	inputPollUpdate();
	updateCallbacks();
	SG().update();
}

void SceneOrchestrator::deactivate() {
}

void SceneOrchestrator::activate() {

	WH::setDropCallback( GDropCallback );
	WH::setResizeWindowCallback( GResizeWindowCallback );
	WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

#ifndef _PRODUCTION_
	Socket::on( "shaderchange",
				std::bind(&SceneOrchestrator::reloadShaders, this, std::placeholders::_1 ) );
#endif

	stateMachine->activate();
}

void SceneOrchestrator::defaults() {
	ImageBuilder{SG().TL(), S::WHITE}.makeDirect( RawImage::WHITE4x4() );
	ImageBuilder{SG().TL(), S::BLACK}.makeDirect( RawImage::BLACK_RGBA4x4 );
	ImageBuilder{SG().TL(), S::NORMAL}.makeDirect( RawImage::NORMAL4x4 );
	ImageBuilder{SG().TL(), S::DEBUG_UV}.makeDirect( RawImage::DEBUG_UV() );
	MaterialBuilder{SG().ML(), S::WHITE_PBR, S::SH}.makeDefault();
	CameraBuilder{SG().CM()}.makeDefault();
}

void SceneOrchestrator::reloadShaders( SocketCallbackDataType _data ) {

	ShaderLiveUpdateMap shadersToUpdate{_data};

	for ( const auto& ss : shadersToUpdate.shaders ) {
		RSG().RR().injectShader( ss.first, ss.second );
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

	SG().CM().update();

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

	RSG().RR().directRenderLoop();

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

void SceneOrchestrator::StateMachine( std::shared_ptr<SceneStateMachineBackEnd> _l ) {
	stateMachine = _l;
}

const std::shared_ptr<ImGuiConsole>& SceneOrchestrator::Console() const {
	return console;
}

void SceneOrchestrator::takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata ) {
    addViewport( RenderTargetType::PBR, SG().CM().get(Name::Sierra),
    		     Rect2f( Vector2f::ZERO, Vector2f{128.0f} ), CameraControls::Fly, BlitType::OffScreen );
    getCamera(Name::Sierra)->center(_box);
    RSG().RR().getTarget(Name::Sierra)->takeScreenShot( _outdata );
}

RenderSceneGraph& SceneOrchestrator::RSG() { return rsg; }

std::shared_ptr<Camera> SceneOrchestrator::getCamera( const std::string& _name ) {
	return SG().CM().getCamera(_name);
}

const cameraRigsMap& SceneOrchestrator::getRigs() const {
	return mRigs;
}

InitializeWindowFlagsT SceneOrchestrator::getLayoutInitFlags() const {
	return stateMachine->getLayoutInitFlags();
}

std::shared_ptr<SceneStateMachineBackEnd> SceneOrchestrator::StateMachine()  {
    return stateMachine;
}

void SceneOrchestrator::script( const std::string& _line ) {
	cq.script(_line);
}

void SceneOrchestrator::addViewport( RenderTargetType _rtt, std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport,
									 CameraControls _cc, BlitType _bt ) {
	if ( mRigs.find(_rig->Name()) == mRigs.end() ) {
		RenderTargetFactory::make( _rtt, _rig, _viewport, _bt, RSG().RR() );
		mRigs[_rig->Name()] = CameraControlFactory::make( _cc, _rig, rsg );
	} else {

		RSG().RR().getTarget(_rig->Name())->getRig()->setViewport(_viewport);
	}
}

CommandQueue& SceneOrchestrator::CQ() {
    return cq;
}

SceneGraph& SceneOrchestrator::SG() {
    return sg;
}
