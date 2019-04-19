//
//  SceneOrchestrator
//
//

#pragma once

#include <unordered_map>
#include <graphics/text_input.hpp>
#include <core/camera_rig.hpp>
#include <core/observer.h>
#include <core/math/vector2f.h>
#include <core/math/rect2f.h>
#include <core/htypes_shared.hpp>
#include <core/game_time.h>
#include <core/streaming_mediator.hpp>
#include <render_scene_graph/render_scene_graph.h>
#include <render_scene_graph/camera_controls.hpp>
#include <graphics/renderer.h>

class MouseInput;
class SceneBridge;
class SceneOrchestrator;
struct ImGuiConsole;
class CameraRig;
class Renderer;
class RenderSceneGraph;
class TextInput;
class CommandQueue;
class CameraControl;
class StreamingMediator;
enum class RenderTargetType;

using PresenterUpdateCallbackFunc = std::function<void(SceneOrchestrator* p)>;
using DragAndDropFunction = std::function<void(SceneOrchestrator* p, const std::string&)>;

struct SceneEventNotifications {
	bool singleMouseTapEvent = false;
};

class SceneOrchestrator : public Observer<MouseInput> {
public:
	SceneOrchestrator( SceneGraph& _sg, RenderSceneGraph& _rsg, TextInput& ti, MouseInput& mi, CommandQueue& cq );
	virtual ~SceneOrchestrator() = default;

    void activate() {
        preActivate();
    }

    void takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata );

	void inputPollUpdate();
	void update();
	void render();
	void enableInputs( bool _bEnabled );
	void deactivate();

    void cmdEnableKeyboard( const std::vector<std::string>& params );
    void cmdDisableKeyboard( const std::vector<std::string>& params );

    void notified( MouseInput& _source, const std::string& generator ) override;

	bool checkKeyPressed( int keyCode );
	bool checkKeyToggled( int keyCode );

	static const std::string DC();

	RenderSceneGraph& RSG();
	SceneGraph& SG();
	CommandQueue& CQ();

	template <typename T>
	void addHttpStream( const std::string& _streamName ) {
		RSG().RR().SSM().addStream<T>( _streamName, avcbTM() );
	}

	void StateMachine( std::shared_ptr<SceneBridge> _l );
	std::shared_ptr<SceneBridge> StateMachine();

	const std::shared_ptr<ImGuiConsole>& Console() const;

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );

	void setDragAndDropFunction( DragAndDropFunction dd );

	void script( const std::string& _line );

public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
    AVInitCallback avcbTM();
	void resetSingleEventNotifications();
	void reloadShaders( SocketCallbackDataTypeConstRef _data );
    void preActivate();
protected:
	std::shared_ptr<SceneBridge> stateMachine;
	SceneGraph& sg;
	RenderSceneGraph& rsg;
    TextInput& ti;
    MouseInput& mi;
	CommandQueue& cq;
	std::shared_ptr<ImGuiConsole> console;
	SceneEventNotifications notifications;
	DragAndDropFunction dragAndDropFunc = nullptr;

private:
	void updateCallbacks();

public:
	static std::vector<PresenterUpdateCallbackFunc> sUpdateCallbacks;

};
