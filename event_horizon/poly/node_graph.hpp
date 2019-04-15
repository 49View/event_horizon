//
// Created by Dado on 2019-04-15.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <boost/signals2.hpp>
#include <core/resources/resource_utils.hpp>

using NodeVariantsSP = GeomSP;

using NodeGraphContainer = std::unordered_map<ResourceRef, NodeVariantsSP>;
using SceneRayIntersectCallback = std::function<void(NodeVariantsSP, float)>;

using NodeGraphConnectParamsSig = NodeVariantsSP;
using NodeGraphConnectFuncSig = void(NodeGraphConnectParamsSig);

class NodeGraph {
public:
    void nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot );

protected:
    NodeGraphContainer nodes;
    boost::signals2::signal<NodeGraphConnectFuncSig> nodeAddSignal;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
};
