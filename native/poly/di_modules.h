#pragma once

#include "core/di.hpp"
#include "scene_graph.h"

namespace di = boost::di;

#define APP_PINJECTOR ( di::bind<CommandQueue>().in(di::singleton), \
                        di::bind<PolySceneGraph>().in(di::singleton) )
