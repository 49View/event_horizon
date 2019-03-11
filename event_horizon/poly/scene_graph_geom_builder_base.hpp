//
// Created by Dado on 2019-03-11.
//

#pragma once

#include <core/descriptors/material.h>
#include <core/builders.hpp>
#include <poly/scene_graph_builder.hpp>

class SceneGraphGeomBaseBuilder : public SceneGraphBuilder, public MaterialBuildable, public DependantBuilder {
public:
    using MaterialBuildable::MaterialBuildable;

    explicit SceneGraphGeomBaseBuilder( SceneGraph& _sg,
                                        const std::string& _shaderName = S::SH,
                                        const std::string& _matName    = S::WHITE_PBR ) :
        SceneGraphBuilder( _sg ),
        MaterialBuildable( _shaderName, _matName ) {}

    SceneGraphGeomBaseBuilder( SceneGraph& _sg, const std::string& _name,
                               const std::string& _shaderName,
                               const std::string& _matName ) :
        SceneGraphBuilder(_sg), MaterialBuildable(_shaderName, _matName), DependantBuilder(_name) {}
};