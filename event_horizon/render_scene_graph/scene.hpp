//
//  Scene
//
//

#pragma once

#include <unordered_map>
#include <graphics/text_input.hpp>
#include "core/observer.h"
#include "core/math/vector2f.h"
#include "core/math/rect2f.h"
#include "core/htypes_shared.hpp"
#include "core/game_time.h"
#include "render_scene_graph.h"

class MouseInput;
class SceneLayout;
class Scene;
struct ImGuiConsole;
class CameraRig;
class Renderer;
class RenderSceneGraph;
class FontManager;
class TextInput;
class CameraManager;
class CommandQueue;
class AABB;
class MaterialManager;
class TextureManager;
class Camera;
class CommandScriptPresenterManager;

namespace PresenterEventFunctionKey {
	const static std::string Activate = "activate";
}

using PresenterUpdateCallbackFunc = std::function<void(Scene* p)>;
using ScenePostActivateFunc = std::function<void(Scene*)>;
using cameraRigsMap = std::unordered_map<std::string, std::shared_ptr<CameraRig>>;

class Scene : public Observer<MouseInput> {
public:
	Scene( Renderer& _rr, RenderSceneGraph& _rsg, FontManager& _fm,
				 TextInput& ti, MouseInput& mi, CameraManager& cm, CommandQueue& cq );

	virtual ~Scene() = default;

    template <typename T>
    void addViewport( const std::string& _name, const Rect2f& _viewport, BlitType _bt ) {
        auto lRig = addTarget<T>( _name, _viewport, _bt, cm );
        mRigs[lRig->Name()] = lRig;
    }

	template<typename T>
	std::shared_ptr<CameraRig> addTarget( const std::string& _name, const Rect2f& _viewport,
										  BlitType _bt, CameraManager& _cm ) {
		auto rig = _cm.getRig( _name );
		if ( !rig ) {
			rig = _cm.addRig( _name, _viewport );
			mTargets.emplace_back( std::make_shared<T>(T{ rig, _viewport, _bt, rr }) );
		}
		return rig;
	}

	std::shared_ptr<RLTarget> getTarget( const std::string& _name );

	void clearTargets();

    void takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata );

	virtual void onTouchUpImpl( [[maybe_unused]] const Vector2f& pos, [[maybe_unused]] ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onSimpleTapImpl( [[maybe_unused]] const Vector2f& pos, [[maybe_unused]] ModifiersKey mod = GMK_MOD_NONE ) {}
	virtual void onDoubleTapImpl( [[maybe_unused]] const Vector2f& pos, [[maybe_unused]] ModifiersKey mod = GMK_MOD_NONE ) {}

	void inputPollUpdate();
	void update();
	void render();
	void enableInputs( bool _bEnabled );
	void addEventFunction( const std::string& _key, std::function<void(Scene*)> _f );
	void deactivate();

    void cmdEnableKeyboard( const std::vector<std::string>& params );
    void cmdDisableKeyboard( const std::vector<std::string>& params );
    void cmdChangeTime( const std::vector<std::string>& params );

    void notified( MouseInput& _source, const std::string& generator ) override;

	bool checkKeyPressed( int keyCode );

	bool activated() const { return mbActivated; }
	static const std::string DC();

	RenderSceneGraph& RSG();
    MaterialManager& ML();
    Renderer& RR();
    CameraManager& CM();
    TextureManager& TM();
	CommandQueue& CQ();
	FontManager& FM();
	std::vector<std::shared_ptr<RLTarget>>& Targets() { return mTargets; }
    std::shared_ptr<Camera> getCamera( const std::string& _name );

	void Layout( std::shared_ptr<SceneLayout> _l );
	std::shared_ptr<SceneLayout> Layout() { return layout; }
	InitializeWindowFlagsT getLayoutInitFlags() const;

	const std::shared_ptr<ImGuiConsole>& Console() const;

	void changeTime( const V3f& _solarTime );

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );
	void postActivate( ScenePostActivateFunc _f ) { postActivateFunc = _f; }
	const cameraRigsMap& getRigs() const {
		return mRigs;
	}

public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
	void activate();

	void reloadShaders( SocketCallbackDataType _data );

protected:
	std::shared_ptr<SceneLayout> layout;
	CameraManager& cm;
	Renderer& rr;
	RenderSceneGraph& rsg;
	FontManager& fm;
    TextInput& ti;
    MouseInput& mi;
	CommandQueue& cq;
	cameraRigsMap mRigs;
	std::vector<std::shared_ptr<RLTarget>> mTargets;

protected:
	bool mbActivated = false;
    std::shared_ptr<CommandScriptPresenterManager> hcs;

    std::unordered_map<std::string, std::function<void(Scene*)> > eventFunctions;
	std::shared_ptr<ImGuiConsole> console;

private:
	void updateCallbacks();
	ScenePostActivateFunc postActivateFunc = nullptr;

public:
	static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;

};
