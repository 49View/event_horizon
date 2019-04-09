//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <string>
#include <core/observer.h>
#include <core/soa_utils.h>
#include <poly/scene_graph.h>
#include <graphics/renderer.h>


struct scene_t;
class AudioManager;
class Renderer;
class VData;

class RenderSceneGraph {
public:
    RenderSceneGraph( Renderer& rr, SceneGraph& _sg );
    virtual ~RenderSceneGraph() = default;

    Renderer& RR();
    SceneGraph& SG() { return sg; }
protected:
//    void updateImpl() override;
//    void addImpl(NodeVariants _geom) override;
//    void removeImpl( const UUID& _uuid ) override;
//    void cmdChangeTimeImpl( const std::vector<std::string>& _params ) override;
//    void changeMaterialTagImpl( const std::vector<std::string>& _params ) override;
//    void changeMaterialColorTagImpl( const std::vector<std::string>& _params ) override;
//    void cmdCreateGeometryImpl( const std::vector<std::string>& _params ) override;
//    void cmdRemoveGeometryImpl( const std::vector<std::string>& _params ) override;
//    void cmdloadObjectImpl( const std::vector<std::string>& _params ) override;
//    void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) override;
//
    void changeMaterialTagCallback( const std::vector<std::string>& _params );
    void changeMaterialColorCallback( const std::vector<std::string>& _params );

    int bake(scene_t *scene);

private:
    Renderer& rr;
    SceneGraph& sg;
    std::shared_ptr<AudioManager> am;
};



