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

using ConnectVoidParamSig = bool;
using ConnectPairStringBoolParamSig = std::pair<std::string, bool>;
using ConnectVoidFuncSig = void(ConnectVoidParamSig);
using ConnectPairStringBoolFuncSig = void(ConnectPairStringBoolParamSig);
using NodeGraphConnectParamsSig = NodeVariantsSP;
using NodeGraphConnectFuncSig = void(NodeGraphConnectParamsSig);
using NodeGraphConnectReplaceFuncSig = void(const std::string&, const std::string&);
using NodeGraphConnectCResourceRefFuncSig = void(CResourceRef);
using NodeGraphConnectChangeMaterialPropertySFuncSig = void(const std::string&, const std::string&, const std::string&);
using NodeGraphConnectChangeMaterialPropertyFFuncSig = void(const std::string&, const std::string&, const float&);
using NodeGraphConnectChangeMaterialPropertyV3fFuncSig = void(const std::string&, const std::string&, const V3f&);
using NodeGraphConnectChangeMaterialPropertyV4fFuncSig = void(const std::string&, const std::string&, const V4f&);

class NodeGraph {
public:
    void nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot );
    void nodeRemoveConnect( const std::function<NodeGraphConnectFuncSig>& _slot );
    void replaceMaterialConnect( const std::function<NodeGraphConnectReplaceFuncSig>& _slot );
    void nodeFullScreenImageConnect( const std::function<NodeGraphConnectCResourceRefFuncSig>& _slot );
    void preloadCompleteConnect( const std::function<ConnectVoidFuncSig>& _slot );
    void propagateDirtyFlagConnect( const std::function<ConnectPairStringBoolFuncSig>& _slot );
    void changeMaterialPropertyConnectString( const std::function<NodeGraphConnectChangeMaterialPropertySFuncSig>& _slot );
    void changeMaterialPropertyConnectFloat( const std::function<NodeGraphConnectChangeMaterialPropertyFFuncSig>& _slot );
    void changeMaterialPropertyConnectV3f( const std::function<NodeGraphConnectChangeMaterialPropertyV3fFuncSig>& _slot );
    void changeMaterialPropertyConnectV4f( const std::function<NodeGraphConnectChangeMaterialPropertyV4fFuncSig>& _slot );

protected:
    NodeGraphContainer nodes;
    boost::signals2::signal<ConnectPairStringBoolFuncSig> propagateDirtyFlagSignal;
    boost::signals2::signal<NodeGraphConnectFuncSig> nodeAddSignal;
    boost::signals2::signal<NodeGraphConnectFuncSig> nodeRemoveSignal;
    boost::signals2::signal<ConnectVoidFuncSig> preloadCompleteSignal;
    boost::signals2::signal<NodeGraphConnectReplaceFuncSig> replaceMaterialSignal;
    boost::signals2::signal<NodeGraphConnectCResourceRefFuncSig> nodeFullScreenImageSignal;
    boost::signals2::signal<NodeGraphConnectChangeMaterialPropertySFuncSig> changeMaterialPropertyStringSignal;
    boost::signals2::signal<NodeGraphConnectChangeMaterialPropertyFFuncSig> changeMaterialPropertyFloatSignal;
    boost::signals2::signal<NodeGraphConnectChangeMaterialPropertyV3fFuncSig> changeMaterialPropertyV3fSignal;
    boost::signals2::signal<NodeGraphConnectChangeMaterialPropertyV4fFuncSig> changeMaterialPropertyV4fSignal;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
    std::unordered_map<std::string, std::string> signalValueMap;
};
