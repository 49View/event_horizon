#pragma once

#include <core/di.hpp>
#include <core/command.hpp>
#include <core/streaming_mediator.hpp>
#include <graphics/renderer.h>
#include <graphics/text_input.hpp>
#include <graphics/mouse_input.hpp>
#include <graphics/shader_manager.h>
#include <graphics/texture_manager.h>
#include <poly/camera_manager.h>
#include "render_scene_graph.h"

namespace di = boost::di;

#define APP_RSGINJECTOR ( di::bind<CommandQueue>().in(di::singleton), \
                          di::bind<FontManager>().in(di::singleton),  \
                          di::bind<RenderSceneGraph>().in(di::singleton),  \
                          di::bind<RenderImageDependencyMaker>().in(di::singleton),  \
                          di::bind<Renderer>().in(di::singleton), \
                          di::bind<TextInput>().in(di::singleton), \
                          di::bind<MouseInput>().in(di::singleton), \
                          di::bind<CameraManager>().in(di::singleton), \
                          di::bind<TextureManager>().in(di::singleton), \
                          di::bind<StreamingMediator>().in(di::singleton), \
                          di::bind<ShaderManager>().in(di::singleton) )
