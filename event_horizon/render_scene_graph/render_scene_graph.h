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

struct HierGeomRenderObserver : public ObserverShared<GeomAsset> {

    explicit HierGeomRenderObserver( Renderer& _rr );
    virtual ~HierGeomRenderObserver() = default;

    std::shared_ptr<PosTexNorTanBinUV2Col3dStrip> generateGeometryVP( const VData& _data );
    void notified( GeomAssetSP _source, const std::string& generator ) override;
private:
    Renderer& rr;
};

struct UIElementRenderObserver : public ObserverShared<UIAsset> {
    explicit UIElementRenderObserver( Renderer& _rr );
    virtual ~UIElementRenderObserver() = default;

    void notified( UIAssetSP _source, const std::string& generator ) override;
private:
    Renderer& rr;
};

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

protected:
    std::shared_ptr<PosTexNorTanBinUV2Col3dStrip> generateGeometryVP( std::shared_ptr<VData> _data );
private:
    Renderer& rr;
    SceneGraph& sg;
    std::shared_ptr<AudioManager> am;
    std::shared_ptr<HierGeomRenderObserver> hierRenderObserver;
    std::shared_ptr<UIElementRenderObserver> uiRenderObserver;
};



