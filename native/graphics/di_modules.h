#pragma once

#include <core/di.hpp>
#include <core/streaming_mediator.hpp>
#include <graphics/renderer.h>
#include <graphics/text_input.hpp>
#include <graphics/mouse_input.hpp>
#include <graphics/shadowmap_manager.h>
#include <graphics/texture_manager.h>
#include <graphics/render_light_manager.h>

namespace di = boost::di;

#define APP_GINJECTOR ( di::bind<CommandQueue>().in(di::singleton), \
                        di::bind<Renderer>().in(di::singleton), \
                        di::bind<ShaderManager>().in(di::singleton), \
                        di::bind<TextureManager>().in(di::singleton), \
                        di::bind<LightManager>().in(di::singleton), \
                        di::bind<Renderer>().in(di::singleton), \
                        di::bind<TextInput>().in(di::singleton), \
                        di::bind<MouseInput>().in(di::singleton) )
