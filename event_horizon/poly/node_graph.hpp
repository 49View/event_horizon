//
// Created by Dado on 2019-04-15.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <boost/signals2.hpp>
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_types.hpp>

using NodeVariantsSP = GeomSP;

using NodeGraphContainer = std::unordered_map<ResourceRef, NodeVariantsSP>;
using SceneRayIntersectCallback = std::function<void(NodeVariantsSP, float)>;

using NodeGraphConnectParamsSig = NodeVariantsSP;
using NodeGraphConnectFuncSig = void(NodeGraphConnectParamsSig);
using NodeGraphConnectReplaceFuncSig = void(const std::string&, const std::string&);
using NodeGraphConnectChangeMaterialPropertyFuncSig = void(const std::string&, const std::string&, const std::string&);

class NodeGraph {
public:
    void nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot );
    void nodeRemoveConnect( const std::function<NodeGraphConnectFuncSig>& _slot );
    void replaceMaterialConnect( const std::function<NodeGraphConnectReplaceFuncSig>& _slot );
    void changeMaterialPropertyConnect( const std::function<NodeGraphConnectChangeMaterialPropertyFuncSig>& _slot );

protected:
    NodeGraphContainer nodes;
    boost::signals2::signal<NodeGraphConnectFuncSig> nodeAddSignal;
    boost::signals2::signal<NodeGraphConnectFuncSig> nodeRemoveSignal;
    boost::signals2::signal<NodeGraphConnectReplaceFuncSig> replaceMaterialSignal;
    boost::signals2::signal<NodeGraphConnectChangeMaterialPropertyFuncSig> changeMaterialPropertySignal;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
};
