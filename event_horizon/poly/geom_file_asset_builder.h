#pragma once

#include <memory>
#include <string>
#include <functional>
#include "core/util.h"
#include "core/callback_dependency.h"
#include "core/image_constants.h"

struct GeomFileAssetBuilder;

struct RBUILDER( GeomFileAssetBuilder, geoms, geom, Binary, BuilderQueryType::NotExact )

};
