//
// Created by Dado on 2019-04-15.
//

#include "node_graph.hpp"

void NodeGraph::nodeAddConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeAddSignal.connect( _slot );
}

void NodeGraph::nodeRemoveConnect( const std::function<NodeGraphConnectFuncSig>& _slot ) {
    nodeRemoveSignal.connect( _slot );
}

void NodeGraph::replaceMaterialConnect( const std::function<NodeGraphConnectReplaceFuncSig>& _slot ) {
    replaceMaterialSignal.connect( _slot );
}
