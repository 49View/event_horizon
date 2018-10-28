//
//  UiPresenter
//
//

#include "ui_presenter.hpp"

#include "graphics/ui/ui_control_manager.hpp"
#include "graphics/camera_manager.h"
#include "graphics/renderer.h"
#include "graphics/render_list.h"
#include "graphics/window_handling.hpp"
#include "graphics/ui/imgui_console.h"

#include <stb/stb_image.h>

std::vector<std::string> UiPresenter::callbackPaths;
Vector2i UiPresenter::callbackResizeWindow = Vector2i(-1, -1);
Vector2i UiPresenter::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> UiPresenter::sUpdateCallbacks;

float sPresenterArrangerLeftFunction( float _value ) {
	return getScreenSizefUI.x() * _value;
}

float sPresenterArrangerRightFunction( float _value ) {
	return getScreenSizefUI.x() * _value;
}

float sPresenterArrangerTopFunction( float _value ) {
	return getScreenSizefUI.y() * ( _value );
}

float sPresenterArrangerBottomFunction( float _value ) {
	return getScreenSizefUI.y() * _value;
}

float sPresenterArrangerLeftFunction3d( float _value ) {
	return getScreenSizef.x() * _value;
}

float sPresenterArrangerRightFunction3d( float _value ) {
	return getScreenSizef.x() * _value;
}

float sPresenterArrangerTopFunction3d( float _value ) {
	return getScreenSizef.y() - ( getScreenSizef.y() * ( _value ) );
}

float sPresenterArrangerBottomFunction3d( float _value ) {
	return getScreenSizef.y() - ( getScreenSizef.y() * ( _value ) );
}

CommandScriptPresenterManager::CommandScriptPresenterManager( UiPresenter& _hm ) {
    addCommandDefinition("enable keyboard", std::bind(&UiPresenter::cmdEnableKeyboard, &_hm, std::placeholders::_1));
    addCommandDefinition("disable keyboard", std::bind(&UiPresenter::cmdDisableKeyboard, &_hm, std::placeholders::_1));
}

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
	ASSERT( count > 0 );

	UiPresenter::callbackPaths.clear();
	for ( auto i = 0; i < count; i++ ) {
		UiPresenter::callbackPaths.emplace_back( std::string(paths[i]) );
	}
}

void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	UiPresenter::callbackResizeWindow = Vector2i{w, h};
}

void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	UiPresenter::callbackResizeFrameBuffer = Vector2i{w, h};
}

UiPresenter::UiPresenter( Renderer& _rr, RenderSceneGraph& _rsg, UiControlManager& _uicm, TextInput& ti, MouseInput& mi,
						  CameraManager& cm, CommandQueue& cq ) :
		cm(cm), rr(_rr), rsg(_rsg), uicm( _uicm ), ti( ti), mi( mi ), cq( cq) {
	hcs = std::make_shared<CommandScriptPresenterManager>(*this);
	cq.registerCommandScript(hcs);
	console = std::make_shared<ImGuiConsole>(cq);
}

void UiPresenter::updateCallbacks() {

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

void UiPresenter::update( GameTime& gt ) {

	if ( !activated() ) {
		activate();
	}

	if ( !mbActivated ) return;

	gameTime = &gt;

	inputPollUpdate();
	updateCallbacks();
}

void UiPresenter::deactivate() {
}

void UiPresenter::activate() {

	if ( !rr.isInitialized() ) return;

	WH::setDropCallback( GDropCallback );
	WH::setResizeWindowCallback( GResizeWindowCallback );
	WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

    //stbi_set_flip_vertically_on_load(true);

	Socket::on( "cloudStorageFileUpdate-shaders/shaders.shd",
			    std::bind(&UiPresenter::reloadShaders, this, std::placeholders::_1 ) );

	MaterialBuilder{"white"}.makeDefault(rsg.ML());

	layout->activate( this );

	if ( const auto& it = eventFunctions.find( PresenterEventFunctionKey::Activate); it != eventFunctions.end() ) {
		it->second(this);
	}
	mbActivated = true;
}

void UiPresenter::reloadShaders( [[maybe_unused]] const rapidjson::Document& _data ) {
	cq.script( "reload shaders" );
}

void UiPresenter::enableInputs( bool _bEnabled ) {
	ti.setEnabled( _bEnabled );
}

void UiPresenter::inputPollUpdate() {

	ViewportTogglesT cvtTggles = ViewportToggles::None;
	// Keyboards
	if ( ti.checkKeyToggleOn( GMK_1 ) ) cvtTggles |= ViewportToggles::DrawWireframe;
    if ( ti.checkKeyToggleOn( GMK_G ) ) cvtTggles |= ViewportToggles::DrawGrid;

	static float camVelocity = 0.0003f;
	static float accumulatedVelocity = .0003f;
	float moveForward = 0.0f;
	float strafe = 0.0f;
	float moveUp = 0.0f;

	if ( ti.checkWASDPressed() != -1 ) {
		camVelocity = 0.003f * gameTime->mCurrTimeStep + accumulatedVelocity;
		if ( ti.checkKeyPressed( GMK_W ) ) moveForward = camVelocity;
		if ( ti.checkKeyPressed( GMK_S ) ) moveForward = -camVelocity;
		if ( ti.checkKeyPressed( GMK_A ) ) strafe = camVelocity;
		if ( ti.checkKeyPressed( GMK_D ) ) strafe = -camVelocity;
		if ( ti.checkKeyPressed( GMK_R ) ) moveUp = -camVelocity;
		if ( ti.checkKeyPressed( GMK_F ) ) moveUp = camVelocity;
		accumulatedVelocity += gameTime->mCurrTimeStep*0.025f;
		if ( camVelocity > 3.50f ) camVelocity = 3.50f;
	} else {
		accumulatedVelocity = 0.0003f;
	}

	cm.updateFromInputData( { cvtTggles,
						      mi.getCurrPosSS(),
	                          mi.isTouchedDown(),
						      mi.getScrollValue(),
							  mi.getCurrMoveDiff( YGestureInvert::No ).dominant()*0.01f,
							  mi.getCurrMoveDiffNorm().dominant(),
							  moveForward, strafe, moveUp} );
    cm.update();
}

bool UiPresenter::checkKeyPressed( int keyCode ) {
	return ti.checkKeyPressed( keyCode );
}

void UiPresenter::notified( MouseInput& _source, const std::string& generator ) {

	if ( generator == "onTouchUp" ) {
		onTouchUpImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else
	if ( generator == "onSingleTap" ) {
		onSimpleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else
	if ( generator == "onDoubleTap" ) {
		onDoubleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	}
	//LOGR( generator.c_str() );
}

void UiPresenter::render() {
	layout->render( this );
}

void UiPresenter::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
	sUpdateCallbacks.push_back( uc );
}

const std::string UiPresenter::DC() {
	return Name::Foxtrot;
}

void UiPresenter::cmdEnableKeyboard( const std::vector<std::string>& params ) {
    WH::enableInputCallbacks();
}

void UiPresenter::cmdDisableKeyboard( const std::vector<std::string>& params ) {
    WH::disableInputCallbacks();
}

void UiPresenter::addEventFunction( const std::string& _key, std::function<void(UiPresenter*)> _f ) {
	eventFunctions[_key] = _f;
}

void UiPresenter::Layout( std::shared_ptr<PresenterLayout> _l ) {
	layout = _l;
}

InitializeWindowFlagsT UiPresenter::getLayoutInitFlags() const {
	return layout->getInitFlags();
}

const std::shared_ptr<ImGuiConsole>& UiPresenter::Console() const {
	return console;
}

void UiPresenter::takeScreenShot( const std::string& _viewportName, ScreenShotContainerPtr _outdata ) {
	rr.getTarget(_viewportName)->takeScreenShot( _outdata );
}

void PresenterLayout::setDragAndDropFunction( DragAndDropFunction dd ) {
	dragAndDropFunc = dd;
}

void initDefaultLayout( const Rect2f& _screenRect, PresenterLayout* _layout, UiPresenter* _target ) {
	_layout->addBox(UiPresenter::DC(), _screenRect.origin().x(), _screenRect.origin().y(),
			_screenRect.origin().x() + _screenRect.size().x(), _screenRect.origin().y() + _screenRect.size().y(),
			CameraControls::Fly );
}

std::shared_ptr<PresenterLayout> PresenterLayout::makeDefault() {
	return std::make_shared<PresenterLayout>(initDefaultLayout);
}

void PresenterLayout::activate( UiPresenter* _target ) {

	initLayout( getScreenRect, this, _target );

	for ( auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Plan2d ) {
			_target->addViewport<RLTargetPlain>( k, v.updateAndGetRect(), v.bt );
		} else if ( v.cc == CameraControls::Walk || v.cc == CameraControls::Fly ) {
			_target->addViewport<RLTargetPBR>( k, v.updateAndGetRect(), v.bt );
			if ( v.cc == CameraControls::Walk ) {
				_target->CM().getCamera(k)->LockAtWalkingHeight(true);
			}
		}
	}
}

void PresenterLayout::resizeCallback( UiPresenter* _target, const Vector2i& _resize ) {
	for ( auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Fly ) {
			auto r = v.updateAndGetRect();
			_target->RR().getTarget( k )->resize( r );
			_target->CM().getRig(k)->setViewport( r );
		}
	}
}
