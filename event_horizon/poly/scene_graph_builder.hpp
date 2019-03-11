//
// Created by Dado on 2019-03-11.
//

#pragma once

class SceneGraph;

class SceneGraphBuilder {
public:
    SceneGraphBuilder( SceneGraph& sg ) : sg( sg ) {}

protected:
    SceneGraph& sg;
};