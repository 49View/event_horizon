//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>

template <typename> class Node;
class GeomData;

using GeomAsset = Node<GeomData>;
using GeomAssetSP = std::shared_ptr<GeomAsset>;
