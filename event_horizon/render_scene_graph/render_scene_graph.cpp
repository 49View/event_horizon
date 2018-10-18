//
// Created by Dado on 08/02/2018.
//

#include "render_scene_graph.h"
#include "core/image_builder.h"
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"
#include "poly/geom_builder.h"

RenderSceneGraph::RenderSceneGraph( Renderer& rr, CommandQueue& cq ) : SceneGraph(cq), rr( rr ), tl(rr.RIDM()) {
    hierRenderObserver = std::make_shared<HierGeomRenderObserver>(rr);
    ml.TL(&tl);
    rr.cacheShadowMapSunPosition(sb.getSunPosition());
}

void RenderSceneGraph::addImpl( std::shared_ptr<HierGeom> _geom ) {
    _geom->subscribeRec(hierRenderObserver);
    _geom->generateSOA();
}

void RenderSceneGraph::changeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {
    rr.mSkybox->invalidate();
    rr.setShadowMapPosition(sb.getSunPosition());
}

void RenderSceneGraph::cmdloadObjectImpl( const std::vector<std::string>& _params ) {
    Vector3f pos = Vector3f::ZERO;
    Vector3f rot = Vector3f::ZERO;
    std::string objName = _params[0];
    if ( _params.size() == 4 || _params.size() == 7 ) {
        pos = { std::stof(_params[1]), std::stof(_params[2]), std::stof(_params[3]) };
    }
    if ( _params.size() == 7 ) {
        rot = { std::stof(_params[4]), std::stof(_params[5]), std::stof(_params[6]) };
    }
    GeomBuilder{GeomBuilderType::file, _params[0] }.at(pos).r(rot).build(*this);
//    GeomBuilder{GeomBuilderType::file, concatParams( _params, 0 ) }.at(pos).r(rot).build(*this);
}

void RenderSceneGraph::changeMaterialTagCallback( const std::vector<std::string>& _params ) {
    std::shared_ptr<PBRMaterial> mat = std::dynamic_pointer_cast<PBRMaterial>(ML().get(concatParams(_params, 1)));
    rr.changeMaterialOnTags( getGeomType( _params[0] ), mat );
}

void RenderSceneGraph::changeMaterialTagImpl( const std::vector<std::string>& _params ) {
    std::string keys = concatenate(" ", { _params.begin()+1, _params.end()} );
    MaterialBuilder{concatParams(_params, 1)}.cc(std::bind( &RenderSceneGraph::changeMaterialTagCallback,
                                                             this,
                                                             _params), _params).rebuild(ML());
}

void RenderSceneGraph::changeMaterialColorCallback( const std::vector<std::string>& _params ) {
    rr.changeMaterialColorOnTags( getGeomType( _params[0] ), CL().getColor(concatParams(_params, 1)) );
}

void RenderSceneGraph::changeMaterialColorTagImpl( const std::vector<std::string>& _params ) {
    ColorBuilder{concatParams(_params, 1)}.cc(std::bind( &RenderSceneGraph::changeMaterialColorCallback,
                                                          this,
                                                          _params), _params).rebuild(CL());
}

void RenderSceneGraph::cmdCreateGeometryImpl( const std::vector<std::string>& _params ) {
    if ( toLower(_params[0]) == "cube" ) {
        GeomBuilder{ ShapeType::Cube }.m( "white" ).build( *this );
    }
}

void HierGeomRenderObserver::notified( std::shared_ptr<HierGeom> _source, const std::string& generator ) {
    rr.invalidateShadowMaps();

    auto lNameHash = std::to_string(_source->Hash());

    auto lvl = rr.VPL( CommandBufferLimits::PBRStart, lNameHash, _source->getLocalHierTransform(), 1.0f );
    VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ rr }.vl(lvl)
            .p( _source->getSOAData()).m(_source->Geom()->getMaterial()).n(lNameHash).g(_source->GHType()).build();
}
