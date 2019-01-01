//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <string>
#include "core/callback_dependency.h"
#include "core/observer.h"
#include "poly/scene_graph.h"
#include "graphics/renderer.h"

struct scene_t;

struct HierGeomRenderObserver : public ObserverShared<GeomAsset> {

    explicit HierGeomRenderObserver( Renderer& _rr ) : rr( _rr ) {}
    virtual ~HierGeomRenderObserver() = default;

    std::shared_ptr<PosTexNorTanBinUV2Col3dStrip> generateGeometryVP( std::shared_ptr<GeomData> _data );
    void notified( GeomAssetSP _source, const std::string& generator ) override;
private:
    Renderer& rr;
};

struct UIElementRenderObserver : public ObserverShared<UIAsset> {
    explicit UIElementRenderObserver( Renderer& _rr ) : rr( _rr ) {}
    virtual ~UIElementRenderObserver() = default;

//    std::shared_ptr<PosTexNorTanBinUV2Col3dStrip> generateGeometryVP( std::shared_ptr<GeomData> _data );
    void notified( UIAssetSP _source, const std::string& generator ) override;
    std::string getShaderType( UIShapeType _st ) const;
private:
    Renderer& rr;
};

class RenderSceneGraph : public SceneGraph {
public:
    RenderSceneGraph( Renderer& rr, CommandQueue& cq, FontManager& fm );
    virtual ~RenderSceneGraph() = default;

    DependencyMaker& TL() override { return tl; }
    Renderer& RR() { return rr; }

protected:
    void addImpl(GeomAssetSP _geom) override;
    void addImpl(UIAssetSP _geom) override;
    void changeMaterialTagImpl( const std::vector<std::string>& _params ) override;
    void changeMaterialColorTagImpl( const std::vector<std::string>& _params ) override;
    void cmdCreateGeometryImpl( const std::vector<std::string>& _params ) override;
    void cmdloadObjectImpl( const std::vector<std::string>& _params ) override;
    void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) override;

    void changeMaterialTagCallback( const std::vector<std::string>& _params );
    void changeMaterialColorCallback( const std::vector<std::string>& _params );

    int bake(scene_t *scene);

private:
    Renderer& rr;
    RenderImageDependencyMaker& tl;
    std::shared_ptr<HierGeomRenderObserver> hierRenderObserver;
    std::shared_ptr<UIElementRenderObserver> uiRenderObserver;
};

