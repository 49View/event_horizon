#pragma once

#include <memory>
#include <string>
#include <functional>
#include "core/util.h"
#include "core/callback_dependency.h"
#include "poly/hier_geom.hpp"
#include "poly/geom_data.hpp"

struct RBUILDER( GeomFileAssetBuilder, geom, geom, Binary, BuilderQueryType::NotExact, GeomData::Version() )

};
