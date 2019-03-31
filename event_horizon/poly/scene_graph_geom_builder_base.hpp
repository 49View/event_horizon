//
// Created by Dado on 2019-03-11.
//

#pragma once

#include <core/names.hpp>
#include <core/builders.hpp>
#include <poly/scene_graph_builder.hpp>

// ### GEOM Remove this class altoghter

class SceneGraphGeomBaseBuilder : public SceneGraphBuilder, public DependantBuilder {
public:

    explicit SceneGraphGeomBaseBuilder( SceneGraph& _sg,
                                        const std::string& _shaderName = S::SH,
                                        const std::string& _matName    = S::WHITE_PBR ) :
        SceneGraphBuilder( _sg ) {}

    SceneGraphGeomBaseBuilder( SceneGraph& _sg, const std::string& _name,
                               const std::string& _shaderName,
                               const std::string& _matName ) :
        SceneGraphBuilder(_sg), DependantBuilder(_name) {}
};