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
std::vector<PresenterUpdateCallbackFunc> UiPresenter::sUpdateCallbacks;

CommandScriptPresenterManager::CommandScriptPresenterManager( UiPresenter& _hm ) {
    addCommandDefinition( "enable keyboard", std::bind(&UiPresenter::cmdEnableKeyboard, &_hm, std::placeholders::_1 ) );
    addCommandDefinition( "disable keyboard", std::bind(&UiPresenter::cmdDisableKeyboard, &_hm, std::placeholders::_1
    ) );
}

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
	ASSERT( count > 0 );

	UiPresenter::callbackPaths.clear();
	for ( auto i = 0; i < count; i++ ) {
		UiPresenter::callbackPaths.push_back( std::string(paths[i]) );
	}
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

//	if ( ti.checkKeyPressed( GMK_LEFT ) ) mi.leftArrowPressed( 2.0f );
//	if ( ti.checkKeyPressed( GMK_RIGHT ) ) mi.rightArrowPressed( 2.0f );

//	if ( ti.checkKeyToggleOn( GMK_V ) ) {
//		VRM.toggleOnOff();
//	}

//	inputPollUpdateImpl();

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

void UiPresenter::takeScreenShot( const std::string& _viewportName ) {
	rr.getTarget(_viewportName)->takeScreenShot();
}

void PresenterLayout::setDragAndDropFunction( DragAndDropFunction dd ) {
	dragAndDropFunc = dd;
}

void initDefaultLayout( const Rect2f& _screenRect, PresenterLayout* _layout, UiPresenter* _target ) {
	_layout->addBox(UiPresenter::DC(), _screenRect, CameraControls::Fly );
}

std::shared_ptr<PresenterLayout> PresenterLayout::makeDefault() {
	return std::make_shared<PresenterLayout>(initDefaultLayout);
}

void PresenterLayout::activate( UiPresenter* _target ) {

	initLayout( getScreenRect, this, _target );

	for ( const auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Plan2d ) {
			_target->addViewport<RLTargetPlain>( k, v.r );
		} else if ( v.cc == CameraControls::Walk || v.cc == CameraControls::Fly ){
			_target->addViewport<RLTargetPBR>( k, v.r );
			if ( v.cc == CameraControls::Walk ) {
				_target->CM().getCamera(k)->LockAtWalkingHeight(true);
			}
		}
	}
}
