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
class RenderOrchestrator;
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
	SceneOrchestrator( SceneGraph& _sg, RenderOrchestrator& _rsg, TextInput& ti, MouseInput& mi, CommandQueue& cq );
	virtual ~SceneOrchestrator() = default;

    void activate() {
        preActivate();
    }

	void update();
	void render();
	void deactivate();

	template <typename T>
	void addHttpStream( const std::string& _streamName ) {
//		RSG().RR().SSM().addStream<T>( _streamName, avcbTM() );
	}

    void addUpdateCallback( PresenterUpdateCallbackFunc uc );

	void setDragAndDropFunction( DragAndDropFunction dd );


public:
	static std::vector<std::string> callbackPaths;
	static Vector2i callbackResizeWindow;
	static Vector2i callbackResizeFrameBuffer;

protected:
    AVInitCallback avcbTM();
	void reloadShaders( SocketCallbackDataTypeConstRef _data );
    void preActivate();
protected:
	std::shared_ptr<SceneBridge> stateMachine;
	SceneGraph& sg;
	RenderOrchestrator& rsg;
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
