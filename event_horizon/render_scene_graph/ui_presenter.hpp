//
//  UiPresenter
//
//

#pragma once

#include "di_modules.h"
#include "core/math/vector2f.h"
#include "core/htypes_shared.hpp"
#include "core/game_time.h"

class MouseInput;
class UiPresenter;

namespace PresenterEventFunctionKey {
	const static std::string Activate = "activate";
}

struct ImGuiConsole;
class PresenterLayout;

using PresenterUpdateCallbackFunc = std::function<void(UiPresenter* p)>;
using DragAndDropFunction = std::function<void(UiPresenter* p, const std::string&)>;
using InitLayoutFunction = std::function<void(PresenterLayout* _layout, UiPresenter*_target)>;
using RenderFunction = std::function<void( UiPresenter* )>;
using RenderLayoutFunction = std::function<void( UiPresenter* _target, const Rect2f&  )>;

using PresenterArrangeFunction = std::function<float( float )>;

//        Rect2f r{ getScreenSizefUI.x() * 0.0f, getScreenSizefUI.y() * uivl.consoleHeight,
//                  getScreenSizefUI.x() * 1.0f, getScreenSizefUI.y()-(getScreenSizefUI.y() * uivl.consoleHeight) };

float sPresenterArrangerLeftFunction( float _value );
float sPresenterArrangerRightFunction( float _value );
float sPresenterArrangerTopFunction( float _value );
float sPresenterArrangerBottomFunction( float _value );

float sPresenterArrangerLeftFunction3d( float _value );
float sPresenterArrangerRightFunction3d( float _value );
float sPresenterArrangerTopFunction3d( float _value );
float sPresenterArrangerBottomFunction3d( float _value );

class PresenterRectArranger {
public:
	PresenterRectArranger() = default;

	explicit PresenterRectArranger( const Rect2f& _r ) {
		setRect( _r );
	}

	PresenterRectArranger( float leftValue, float rightValue, float topValue, float bottomValue ) : leftValue(
			leftValue ), rightValue( rightValue ), topValue( topValue ), bottomValue( bottomValue ) {}

	PresenterRectArranger( const PresenterArrangeFunction& leftFunc, const PresenterArrangeFunction& rightFunc,
						   const PresenterArrangeFunction& topFunc, const PresenterArrangeFunction& bottomFunc,
						   float leftValue, float rightValue, float topValue, float bottomValue ) :
						   leftFunc( leftFunc ), rightFunc( rightFunc ), topFunc( topFunc ), bottomFunc( bottomFunc ),
						   leftValue(leftValue), rightValue(rightValue), topValue(topValue), bottomValue(bottomValue) {}

	void set() {
		rect.setLeft(leftFunc(leftValue));
		rect.setBottom( bottomFunc(bottomValue) );
		rect.setRight(rightFunc(rightValue));
		rect.setTop( topFunc(topValue) );
	}

	const Rect2f& getRect() const {
		return rect;
	}

	void setRect( const Rect2f& rect ) {
		PresenterRectArranger::rect = rect;
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
};

class PresenterLayout {
public:
	explicit PresenterLayout( InitLayoutFunction&& initLayout,
							  RenderFunction&& _renderFunction = nullptr,
					 		  DragAndDropFunction&& _dd = nullptr,
					 		  InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal ) :
			  initLayout( initLayout ), renderFunction(_renderFunction), dragAndDropFunc(_dd), initFlags( initFlags ) {
	}

	struct Boxes {
		Rect2f updateAndGetRect() {
			rectArranger.set();
			return rectArranger.getRect();
		}
		Rect2f getRect() const {
			return rectArranger.getRect();
		}
		PresenterRectArranger rectArranger;
		CameraControls cc= CameraControls::Fly;
		BlitType bt = BlitType::OnScreen;
		RenderLayoutFunction renderFunction = nullptr;
		static const Boxes INVALID;
	};

	void addBox( const std::string& _name, float _l, float _r, float _t, float _b, RenderLayoutFunction&& rlf ) {
		boxes[_name] = { { _l, _r, _t, _b}, CameraControls::Edit2d, BlitType::OnScreen, rlf };
	}

	void addBox( const std::string& _name, float _l, float _r, float _t, float _b, CameraControls _cc  ) {
		boxes[_name] = { {
			sPresenterArrangerLeftFunction3d,
			sPresenterArrangerRightFunction3d,
			sPresenterArrangerTopFunction3d,
			sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, _cc, BlitType::OnScreen, nullptr };
	}

    void addOffScreenBox(const std::string& _name, const Vector2f& _s, CameraControls _cc = CameraControls::Fly ) {
        boxes[_name] = { PresenterRectArranger{Rect2f(Vector2f::ZERO, _s)}, _cc, BlitType::OffScreen };
    }

    const Boxes& Box( const std::string& _key ) const {
		if ( const auto& it = boxes.find(_key); it != boxes.end() ) {
			return it->second;
		}
		return Boxes::INVALID;
	}

	Rect2f BoxUpdateAndGet( const std::string& _key ) {
		if ( const auto& it = boxes.find(_key); it != boxes.end() ) {
			return it->second.updateAndGetRect();
		}
		return Rect2f::INVALID;
	}

	InitializeWindowFlagsT getInitFlags() const {
		return initFlags;
	}

	void setInitFlags( InitializeWindowFlagsT initFlags ) {
		PresenterLayout::initFlags = initFlags;
	}

	static std::shared_ptr<PresenterLayout> makeDefault();

	void resizeCallback( UiPresenter* _target, const Vector2i& _resize );

	void render( UiPresenter* _target ) {
		for ( const auto& [k,v] : boxes ) {
			if ( v.renderFunction ) {
				v.renderFunction( _target, BoxUpdateAndGet(k) );
			}
		}
//		if ( renderFunction ) renderFunction( _target );
	}

	void setDragAndDropFunction( DragAndDropFunction dd );

private:
	void activate( UiPresenter* _target );

	std::unordered_map<std::string, Boxes> boxes;
	InitLayoutFunction initLayout;
	RenderFunction renderFunction;
	DragAndDropFunction dragAndDropFunc;
	InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal;

	friend class UiPresenter;
};

class CommandScriptPresenterManager : public CommandScript {
public:
    explicit CommandScriptPresenterManager( UiPresenter& hm );
    virtual ~CommandScriptPresenterManager() = default;
};

class UiPresenter : public Observer<MouseInput> {
public:
	UiPresenter( Renderer& _rr, RenderSceneGraph& _rsg, UiControlManager& _uicm,
				 TextInput& ti, MouseInput& mi, CameraManager& cm, CommandQueue& cq );

	virtual ~UiPresenter() = default;

    template <typename T>
    void addViewport( const std::string& _name, const Rect2f& _viewport, BlitType _bt ) {
        auto lRig = rr.addTarget<T>( _name, _viewport, _bt, cm );
        mRigs[lRig->Name()] = lRig;
    }

    void takeScreenShot( const std::string& _viewportName, ScreenShotContainerPtr _outdata );

	virtual void onTouchUpImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onSimpleTapImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onDoubleTapImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}

	void inputPollUpdate();
	void update( GameTime& gt );
	void render();
	void enableInputs( bool _bEnabled );
	void addEventFunction( const std::string& _key, std::function<void(UiPresenter*)> _f );
	void deactivate();

    void cmdEnableKeyboard( const std::vector<std::string>& params );
    void cmdDisableKeyboard( const std::vector<std::string>& params );

    void notified( MouseInput& _source, const std::string& generator ) override;

	bool checkKeyPressed( int keyCode );

	bool activated() const { return mbActivated; }
	static const std::string DC();

	RenderSceneGraph& RSG() { return rsg; }
    MaterialManager& ML() { return rsg.ML(); }
    Renderer& RR() { return rr; }
    CameraManager& CM() { return cm; }
    TextureManager& TM() { return rr.TM(); }
	CommandQueue& CQ() { return cq; }
    std::shared_ptr<Camera> getCamera( const std::string& _name ) { return CM().getCamera(_name); }

	void Layout( std::shared_ptr<PresenterLayout> _l );
	std::shared_ptr<PresenterLayout> Layout() { return layout; }
	InitializeWindowFlagsT getLayoutInitFlags() const;

	const std::shared_ptr<ImGuiConsole>& Console() const;

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );

public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
	void activate();

	void reloadShaders(const rapidjson::Document& _data);

protected:
	GameTime* gameTime;
	std::shared_ptr<PresenterLayout> layout;
	CameraManager& cm;
	Renderer& rr;
	RenderSceneGraph& rsg;
	UiControlManager& uicm;
    TextInput& ti;
    MouseInput& mi;
	CommandQueue& cq;
    std::unordered_map<std::string, std::shared_ptr<CameraRig>> mRigs;
	bool mbActivated = false;
    std::shared_ptr<CommandScriptPresenterManager> hcs;

    std::unordered_map<std::string, std::function<void(UiPresenter*)> > eventFunctions;
	std::shared_ptr<ImGuiConsole> console;

private:
	void updateCallbacks();

public:
	static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;

};
