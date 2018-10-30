//
//  Scene
//
//

#pragma once

#include "di_modules.h"
#include "core/math/vector2f.h"
#include "core/htypes_shared.hpp"
#include "core/game_time.h"

class MouseInput;
class SceneLayout;
class Scene;
struct ImGuiConsole;

namespace PresenterEventFunctionKey {
	const static std::string Activate = "activate";
}

using PresenterUpdateCallbackFunc = std::function<void(Scene* p)>;
using cameraRigsMap = std::unordered_map<std::string, std::shared_ptr<CameraRig>>;

class CommandScriptPresenterManager : public CommandScript {
public:
    explicit CommandScriptPresenterManager( Scene& hm );
    virtual ~CommandScriptPresenterManager() = default;
};

class Scene : public Observer<MouseInput> {
public:
	Scene( Renderer& _rr, RenderSceneGraph& _rsg, UiControlManager& _uicm,
				 TextInput& ti, MouseInput& mi, CameraManager& cm, CommandQueue& cq );

	virtual ~Scene() = default;

    template <typename T>
    void addViewport( const std::string& _name, const Rect2f& _viewport, BlitType _bt ) {
        auto lRig = rr.addTarget<T>( _name, _viewport, _bt, cm );
        mRigs[lRig->Name()] = lRig;
    }

    void takeScreenShot( const AABB& _box, ScreenShotContainerPtr _outdata );

	virtual void onTouchUpImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onSimpleTapImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onDoubleTapImpl( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE ) {}

	void inputPollUpdate();
	void update( GameTime& gt );
	void render();
	void enableInputs( bool _bEnabled );
	void addEventFunction( const std::string& _key, std::function<void(Scene*)> _f );
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

	void Layout( std::shared_ptr<SceneLayout> _l );
	std::shared_ptr<SceneLayout> Layout() { return layout; }
	InitializeWindowFlagsT getLayoutInitFlags() const;

	const std::shared_ptr<ImGuiConsole>& Console() const;

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );

	const cameraRigsMap& getRigs() const {
		return mRigs;
	}

public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
	void activate();

	void reloadShaders(const rapidjson::Document& _data);

protected:
	GameTime* gameTime;
	std::shared_ptr<SceneLayout> layout;
	CameraManager& cm;
	Renderer& rr;
	RenderSceneGraph& rsg;
	UiControlManager& uicm;
    TextInput& ti;
    MouseInput& mi;
	CommandQueue& cq;
	cameraRigsMap mRigs;

protected:
	bool mbActivated = false;
    std::shared_ptr<CommandScriptPresenterManager> hcs;

    std::unordered_map<std::string, std::function<void(Scene*)> > eventFunctions;
	std::shared_ptr<ImGuiConsole> console;

private:
	void updateCallbacks();

public:
	static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;

};
