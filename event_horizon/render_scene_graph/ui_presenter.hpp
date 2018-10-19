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

namespace Name {
	const static std::string Alpha	  = "Alpha	 ";
	const static std::string Bravo	  = "Bravo	 ";
	const static std::string Charlie  = "Charlie ";
	const static std::string Delta	  = "Delta	 ";
	const static std::string Echo	  = "Echo	 ";
	const static std::string Foxtrot  = "Foxtrot ";
	const static std::string Golf	  = "Golf	 ";
	const static std::string Hotel	  = "Hotel	 ";
	const static std::string India	  = "India	 ";
	const static std::string Juliett  = "Juliett ";
	const static std::string Kilo	  = "Kilo	 ";
	const static std::string Lima	  = "Lima	 ";
	const static std::string Mike	  = "Mike	 ";
	const static std::string November = "November";
	const static std::string Oscar	  = "Oscar	 ";
	const static std::string Papa	  = "Papa	 ";
	const static std::string Quebec   = "Quebec  ";
	const static std::string Romeo	  = "Romeo	 ";
	const static std::string Sierra   = "Sierra  ";
	const static std::string Tango	  = "Tango	 ";
	const static std::string Uniform  = "Uniform ";
	const static std::string Victor   = "Victor  ";
	const static std::string Whiske   = "Whiske  ";
	const static std::string X_Ray	  = "X_Ray	 ";
	const static std::string Yankee   = "Yankee  ";
	const static std::string Zulu	  = "Zulu	 ";
}

struct ImGuiConsole;
class PresenterLayout;

using PresenterUpdateCallbackFunc = std::function<void(UiPresenter* p)>;
using DragAndDropFunction = std::function<void(UiPresenter* p, const std::string&)>;
using InitLayoutFunction = std::function<void(
		const Rect2f& _screenRect, PresenterLayout* _layout, UiPresenter*_target)>;
using RenderLayoutFunction = std::function<void( UiPresenter* _target )>;

class PresenterLayout {
public:
	PresenterLayout( InitLayoutFunction initLayout,
					 RenderLayoutFunction _renderFunction = nullptr,
					 DragAndDropFunction _dd = nullptr,
					 InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal ) :
			  initLayout( initLayout ), renderFunction(_renderFunction), dragAndDropFunc(_dd), initFlags( initFlags ) {
	}

	struct Boxes {
		Rect2f r;
		CameraControls cc= CameraControls::Fly;
		BlitType bt = BlitType::OnScreen;
	};

	void addBox( const std::string& _name, const Rect2f& _r, CameraControls _cc, BlitType _bt = BlitType::OnScreen ) {
		boxes[_name] = { _r, _cc, _bt };
	}

    void addOffScreenBox(const std::string& _name, const Vector2f& _s, CameraControls _cc = CameraControls::Fly ) {
        boxes[_name] = { Rect2f(Vector2f::ZERO, _s), _cc, BlitType::OffScreen };
    }

    InitializeWindowFlagsT getInitFlags() const {
		return initFlags;
	}

	void setInitFlags( InitializeWindowFlagsT initFlags ) {
		PresenterLayout::initFlags = initFlags;
	}

	static std::shared_ptr<PresenterLayout> makeDefault();

	void render( UiPresenter* _target ) {
		if ( renderFunction ) renderFunction( _target );
	}

	void setDragAndDropFunction( DragAndDropFunction dd );

private:
	void activate( UiPresenter* _target );

	std::unordered_map<std::string, Boxes> boxes;
	InitLayoutFunction initLayout;
	RenderLayoutFunction renderFunction;
	DragAndDropFunction dragAndDropFunc;
	InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal;

	friend class UiPresenter;
};

class CommandScriptPresenterManager : public CommandScript {
public:
    CommandScriptPresenterManager( UiPresenter& hm );
    virtual ~CommandScriptPresenterManager() {}
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
	InitializeWindowFlagsT getLayoutInitFlags() const;

	const std::shared_ptr<ImGuiConsole>& Console() const;

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );

public:
	static std::vector<std::string> callbackPaths;

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
