#pragma once

#include "core/di.hpp"
#include "core/command.hpp"
#include "core/font_manager.h"
#include "renderer.h"
#include "text_input.hpp"
#include "mouse_input.hpp"
#include "shader_manager.h"
#include "camera_manager.h"
#include "texture_manager.h"
#include "ui/ui_control_manager.hpp"

namespace di = boost::di;

#define APP_GINJECTOR ( di::bind<CommandQueue>().in(di::singleton), di::bind<UiControlManager>().in(di::singleton), \
                        di::bind<FontManager>().in(di::singleton), \
                        di::bind<Renderer>().in(di::singleton), \
                        di::bind<TextInput>().in(di::singleton), di::bind<MouseInput>().in(di::singleton), \
                        di::bind<TextureManager>().in(di::singleton), \
                        di::bind<ShaderManager>().in(di::singleton), di::bind<CameraManager>().in(di::singleton))
