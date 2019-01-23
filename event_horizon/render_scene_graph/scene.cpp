//
//  Scene
//
//

#include "scene.hpp"

#include "graphics/camera_manager.h"
#include "graphics/renderer.h"
#include "graphics/render_list.h"
#include "graphics/camera_rig.hpp"
#include "graphics/window_handling.hpp"
#include "graphics/ui/imgui_console.h"

#include <render_scene_graph/scene_layout.h>
#include <render_scene_graph/render_scene_graph.h>

#include <stb/stb_image.h>

#include "di_modules.h"

std::vector<std::string> Scene::callbackPaths;
Vector2i Scene::callbackResizeWindow = Vector2i(-1, -1);
Vector2i Scene::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> Scene::sUpdateCallbacks;

class CommandScriptPresenterManager : public CommandScript {
public:
	explicit CommandScriptPresenterManager( Scene& hm );
	virtual ~CommandScriptPresenterManager() = default;
};

CommandScriptPresenterManager::CommandScriptPresenterManager( Scene& _hm ) {
    addCommandDefinition("enable keyboard", std::bind(&Scene::cmdEnableKeyboard, &_hm, std::placeholders::_1));
    addCommandDefinition("disable keyboard", std::bind(&Scene::cmdDisableKeyboard, &_hm, std::placeholders::_1));
	addCommandDefinition("change time", std::bind(&Scene::cmdChangeTime, &_hm, std::placeholders::_1 ));
}

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
	ASSERT( count > 0 );

	Scene::callbackPaths.clear();
	for ( auto i = 0; i < count; i++ ) {
		Scene::callbackPaths.emplace_back( std::string(paths[i]) );
	}
}

void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	Scene::callbackResizeWindow = Vector2i{w, h};
}

void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	Scene::callbackResizeFrameBuffer = Vector2i{w, h};
}

Scene::Scene( Renderer& _rr, RenderSceneGraph& _rsg, FontManager& _fm, TextInput& ti, MouseInput& mi,
						  CameraManager& cm, CommandQueue& cq, StreamingMediator& _ssm ) :
		cm(cm), rr(_rr), rsg(_rsg), fm( _fm ), ti( ti), mi( mi ), cq( cq), ssm(_ssm) {
	hcs = std::make_shared<CommandScriptPresenterManager>(*this);
	cq.registerCommandScript(hcs);
	console = std::make_shared<ImGuiConsole>(cq);
}

void Scene::updateCallbacks() {

	if ( !sUpdateCallbacks.empty() ) {
		for ( auto& c : sUpdateCallbacks ) {
			c( this );
		}
		sUpdateCallbacks.clear();
	}

	if ( !callbackPaths.empty() ) {
		for ( auto& path : callbackPaths ) {
			if ( layout->dragAndDropFunc ) layout->dragAndDropFunc( this, path );
		}
		callbackPaths.clear();
	}

	if ( callbackResizeWindow.x() > 0 && callbackResizeWindow.y() > 0 ) {
		LOGR("Resized window: [%d, %d]", callbackResizeWindow.x(), callbackResizeWindow.y() );
		callbackResizeWindow = Vector2i{-1, -1};
	}

	if ( callbackResizeFrameBuffer.x() > 0 && callbackResizeFrameBuffer.y() > 0 ) {
		WH::gatherMainScreenInfo();
		RR().resetDefaultFB();
		layout->resizeCallback( this, callbackResizeFrameBuffer );
		LOGR("Resized framebuffer: [%d, %d]", callbackResizeFrameBuffer.x(), callbackResizeFrameBuffer.y() );
		callbackResizeFrameBuffer = Vector2i{-1, -1};
	}

}

void Scene::update() {

	if ( !activated() ) {
		activate();
		if ( postActivateFunc ) postActivateFunc( this );
	}

	if ( !mbActivated ) return;

	inputPollUpdate();
	updateCallbacks();
	RSG().update();
}

void Scene::deactivate() {
}

void Scene::activate() {

	WH::setDropCallback( GDropCallback );
	WH::setResizeWindowCallback( GResizeWindowCallback );
	WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

    //stbi_set_flip_vertically_on_load(true);

	Socket::on( "shaderchange",
				std::bind(&Scene::reloadShaders, this, std::placeholders::_1 ) );

	MaterialBuilder{"white"}.makeDefault(rsg.ML());
	ImageBuilder{"white"}.makeDirect( rsg.TL(), RawImage::WHITE4x4() );
	ImageBuilder{"debug_uv"}.makeDirect( rsg.TL(), RawImage::DEBUG_UV() );

	CQ().script("change time 14:00");
	layout->activate( this );

	if ( const auto& it = eventFunctions.find( PresenterEventFunctionKey::Activate); it != eventFunctions.end() ) {
		it->second(this);
	}
	mbActivated = true;
}

void Scene::reloadShaders( SocketCallbackDataType _data ) {

	ShaderLiveUpdateMap shadersToUpdate{_data};

	for ( const auto& ss : shadersToUpdate.shaders ) {
		rr.injectShader( ss.first, ss.second );
	}
	cq.script( "reload shaders" );
}

void Scene::enableInputs( bool _bEnabled ) {
	ti.setEnabled( _bEnabled );
}

void Scene::resetSingleEventNotifications() {
    notifications.singleMouseTapEvent = false;
}

void Scene::inputPollUpdate() {

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

bool Scene::checkKeyPressed( int keyCode ) {
	return ti.checkKeyPressed( keyCode );
}

void Scene::notified( MouseInput& _source, const std::string& generator ) {

	if ( generator == "onTouchUp" ) {
//		onTouchUpImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else if ( generator == "onSingleTap" ) {
		notifications.singleMouseTapEvent = true;
	} else if ( generator == "onDoubleTap" ) {
//		onDoubleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	}
	//LOGR( generator.c_str() );
}

void Scene::render() {
	layout->render( this );

	for ( auto& [k,v] : mRigs ) {
		v->renderControls(this);
	}
}

void Scene::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
	sUpdateCallbacks.push_back( uc );
}

const std::string Scene::DC() {
	return Name::Foxtrot;
}

void Scene::cmdEnableKeyboard( const std::vector<std::string>& params ) {
    WH::enableInputCallbacks();
}

void Scene::cmdDisableKeyboard( const std::vector<std::string>& params ) {
    WH::disableInputCallbacks();
}

void Scene::cmdChangeTime( const std::vector<std::string>& _params ) {
	RSG().SB().buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
	changeTime( RSG().SB().getSunPosition() );
}

void Scene::addEventFunction( const std::string& _key, std::function<void(Scene*)> _f ) {
	eventFunctions[_key] = _f;
}

void Scene::Layout( std::shared_ptr<SceneLayout> _l ) {
	layout = _l;
}

InitializeWindowFlagsT Scene::getLayoutInitFlags() const {
	return layout->getInitFlags();
}

const std::shared_ptr<ImGuiConsole>& Scene::Console() const {
	return console;
}

void Scene::takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata ) {
    addViewport<RLTargetPBR>( Name::Sierra, Rect2f( Vector2f::ZERO, Vector2f{128.0f} ), CameraControls::Fly,
    		                  BlitType::OffScreen );
    getCamera(Name::Sierra)->center(_box);
    getTarget(Name::Sierra)->takeScreenShot( _outdata );
}

void Scene::clearTargets() {
	for ( const auto& target : mTargets ) {
		target->clearCB();
	}
}

std::shared_ptr<RLTarget> Scene::getTarget( const std::string& _name ) {

	for ( auto& t : mTargets ) {
		if ( t->cameraRig->Name() == _name ) return t;
	}

	return nullptr;
}

void Scene::changeTime( const V3f& _solar ) {
	for ( auto& t : mTargets ) {
		t->changeTime( _solar );
	}
}

RenderSceneGraph& Scene::RSG() { return rsg; }
MaterialManager& Scene::ML() { return rsg.ML(); }
Renderer& Scene::RR() { return rr; }
CameraManager& Scene::CM() { return cm; }
TextureManager& Scene::TM() { return rr.TM(); }
CommandQueue& Scene::CQ() { return cq; }
FontManager& Scene::FM() { return fm; }
StreamingMediator& Scene::SSM() { return ssm; }

std::shared_ptr<Camera> Scene::getCamera( const std::string& _name ) { return CM().getCamera(_name); }

const cameraRigsMap& Scene::getRigs() const {
	return mRigs;
}

void Scene::script( const std::string& _commandLine ) {
	CQ().script( _commandLine );
}



