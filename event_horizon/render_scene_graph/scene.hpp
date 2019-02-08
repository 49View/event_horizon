//
//  SceneOrchestrator
//
//

#pragma once

#include <unordered_map>
#include <graphics/text_input.hpp>
#include <graphics/camera_manager.h>
#include <core/observer.h>
#include <core/math/vector2f.h>
#include <core/math/rect2f.h>
#include <core/htypes_shared.hpp>
#include <core/game_time.h>
#include <core/streaming_mediator.hpp>
#include <render_scene_graph/render_scene_graph.h>
#include <render_scene_graph/camera_controls.hpp>

class MouseInput;
class SceneLayout;
class SceneOrchestrator;
struct ImGuiConsole;
class CameraRig;
class Renderer;
class RenderSceneGraph;
class FontManager;
class TextInput;
class CameraManager;
class CommandQueue;
class MaterialManager;
class TextureManager;
class Camera;
class CameraControl;
class CommandScriptPresenterManager;
class StreamingMediator;

namespace PresenterEventFunctionKey {
	const static std::string Activate = "activate";
}

using PresenterUpdateCallbackFunc = std::function<void(SceneOrchestrator* p)>;
using ScenePostActivateFunc = std::function<void(SceneOrchestrator*)>;
using cameraRigsMap = std::unordered_map<std::string, std::shared_ptr<CameraControl>>;

struct SceneEventNotifications {
	bool singleMouseTapEvent = false;
};

class SceneOrchestrator : public Observer<MouseInput> {
public:
	SceneOrchestrator( Renderer& _rr, RenderSceneGraph& _rsg, FontManager& _fm, TextInput& ti, MouseInput& mi,
		   CameraManager& cm, CommandQueue& cq, StreamingMediator& _ssm );

	virtual ~SceneOrchestrator() = default;

    template <typename T>
    void addViewport( const std::string& _name, const Rect2f& _viewport, CameraControls _cc, BlitType _bt ) {
        auto lRig = addTarget<T>( _name, _viewport, _bt, cm );
        mRigs[_name] = CameraControlFactory::make( _cc, lRig, rsg );
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

	void inputPollUpdate();
	void update();
	void render();
	void enableInputs( bool _bEnabled );
	void addEventFunction( const std::string& _key, std::function<void(SceneOrchestrator*)> _f );
	void deactivate();

	void script( const std::string& _commandLine );
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
	StreamingMediator& SSM();
	std::vector<std::shared_ptr<RLTarget>>& Targets() { return mTargets; }
    std::shared_ptr<Camera> getCamera( const std::string& _name );

	template <typename T>
	void addHttpStream( const std::string& _streamName ) {
		SSM().addStream<T>( _streamName, avcbTM );
	}

	void Layout( std::shared_ptr<SceneLayout> _l );
	std::shared_ptr<SceneLayout> Layout() { return layout; }
	InitializeWindowFlagsT getLayoutInitFlags() const;

	const std::shared_ptr<ImGuiConsole>& Console() const;

	void changeTime( const V3f& _solarTime );

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );
	void postActivate( ScenePostActivateFunc _f ) { postActivateFunc = _f; }
	const cameraRigsMap& getRigs() const;

public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
	void resetSingleEventNotifications();
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
	StreamingMediator& ssm;
	cameraRigsMap mRigs;
	std::vector<std::shared_ptr<RLTarget>> mTargets;
	bool mbActivated = false;
    std::shared_ptr<CommandScriptPresenterManager> hcs;
    std::unordered_map<std::string, std::function<void(SceneOrchestrator*)> > eventFunctions;
	std::shared_ptr<ImGuiConsole> console;
	SceneEventNotifications notifications;

private:
	void updateCallbacks();
	ScenePostActivateFunc postActivateFunc = nullptr;

public:
	static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;

};
