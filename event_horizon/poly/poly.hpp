//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>

template <typename> class Hier;
class GeomData;

using GeomAsset = Hier<GeomData>;
using GeomAssetSP = std::shared_ptr<GeomAsset>;
