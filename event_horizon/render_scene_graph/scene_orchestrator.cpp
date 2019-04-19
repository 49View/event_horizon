//
//  SceneOrchestrator
//
//

#include "scene_orchestrator.hpp"

#include <core/camera.h>
#include <core/camera_rig.hpp>
#include <graphics/renderer.h>
#include <graphics/render_list.h>
#include <graphics/shader_manager.h>
#include <graphics/window_handling.hpp>
#include <graphics/ui/imgui_console.h>
#include <graphics/render_targets.hpp>

#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/render_scene_graph.h>

#include <core/resources/resource_manager.hpp>

#include <stb/stb_image.h>

std::vector<std::string> SceneOrchestrator::callbackPaths;
Vector2i SceneOrchestrator::callbackResizeWindow = Vector2i(-1, -1);
Vector2i SceneOrchestrator::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> SceneOrchestrator::sUpdateCallbacks;

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
        RSG().resizeCallback( callbackResizeFrameBuffer );
		callbackResizeFrameBuffer = Vector2i{-1, -1};
	}

}

void SceneOrchestrator::update() {
//	stateMachine->run();
	inputPollUpdate();
	updateCallbacks();
	SG().update();
}

void SceneOrchestrator::deactivate() {
}

void SceneOrchestrator::preActivate() {
    WH::setDropCallback( GDropCallback );
    WH::setResizeWindowCallback( GResizeWindowCallback );
    WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

#ifndef _PRODUCTION_
    Socket::on( "shaderchange",
                std::bind(&SceneOrchestrator::reloadShaders, this, std::placeholders::_1 ) );
#endif
}

void SceneOrchestrator::reloadShaders( SocketCallbackDataTypeConstRef _data ) {

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

//	stateMachine->render( this );
//
//	for ( auto& [k,v] : StateMachine()->getRigs() ) {
//		v->renderControls();
//	}
//
//	RSG().RR().directRenderLoop();

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

void SceneOrchestrator::StateMachine( std::shared_ptr<SceneBridge> _l ) {
	stateMachine = _l;
}

const std::shared_ptr<ImGuiConsole>& SceneOrchestrator::Console() const {
	return console;
}

void SceneOrchestrator::takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata ) {
//    addViewport<CameraControlFly>( RenderTargetType::PBR, Name::Sierra,
//    		     Rect2f( Vector2f::ZERO, Vector2f{128.0f} ), BlitType::OffScreen );
//    getCamera(Name::Sierra)->center(_box);
//    RSG().RR().getTarget(Name::Sierra)->takeScreenShot( _outdata );
}

RenderSceneGraph& SceneOrchestrator::RSG() { return rsg; }

std::shared_ptr<SceneBridge> SceneOrchestrator::StateMachine()  {
    return stateMachine;
}

void SceneOrchestrator::script( const std::string& _line ) {
	cq.script(_line);
}

CommandQueue& SceneOrchestrator::CQ() {
    return cq;
}

SceneGraph& SceneOrchestrator::SG() {
    return sg;
}

AVInitCallback SceneOrchestrator::avcbTM() {
    return std::bind(&TextureManager::preparingStremingTexture,
                     RSG().RR().TM().get(),
                     std::placeholders::_1,
                     std::placeholders::_2);
}
