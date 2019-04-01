//
// Created by Dado on 08/02/2018.
//

#include "render_scene_graph.h"
#include <poly/resources/resource_utils.hpp>
#include <poly/resources/resource_manager.hpp>
#include <core/raw_image.h>
#include <core/node.hpp>
#include <event_horizon/poly/resources/geom_builder.h>
#include <event_horizon/poly/resources/ui_shape_builder.h>
#include <graphics/renderer.h>
#include <graphics/vp_builder.hpp>
#include <graphics/audio/audio_manager_openal.hpp>

std::shared_ptr<PosTexNorTanBinUV2Col3dStrip>
RenderSceneGraph::generateGeometryVP( std::shared_ptr<VData> _data ) {
    if ( _data->numIndices() < 3 ) return nullptr;

    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[_data->numIndices()] );
    std::memcpy( _indices.get(), _data->Indices(), _data->numIndices() * sizeof( int32_t ));
    auto SOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( _data->numVerts(), PRIMITIVE_TRIANGLES,
                                                                   VFVertexAllocation::PreAllocate, _data->numIndices(),
                                                                   _indices );
    for ( int32_t t = 0; t < _data->numVerts(); t++ ) {
        SOAData->addVertex( _data->vertexAt( t ), _data->uvAt( t ), _data->uv2At( t ), _data->normalAt( t ),
                            _data->tangentAt( t ), _data->binormalAt( t ), _data->colorAt(t) );
    }
    return SOAData;
}

RenderSceneGraph::RenderSceneGraph( Renderer& rr, SceneGraph& _sg ) :
                                    rr( rr ), sg(_sg) {
    hierRenderObserver = std::make_shared<HierGeomRenderObserver>(rr);
    uiRenderObserver = std::make_shared<UIElementRenderObserver>(rr);

    sg.TL().connect( [this](const ResourceSignalsAddSignature<RawImage>& _val ) {
        this->RR().tm.addTextureWithData( *std::get<0>(_val).get(), std::get<1>(_val) );
    });

    sg.ML().connect( [](const ResourceSignalsAddSignature<Material>& _val ) {
        LOGRS( "Adding Material " << std::get<1>(_val)[0] );
    });

    sg.VL().connect( [this](const ResourceSignalsAddSignature<VData>& _val ) {
        LOGRS( "Adding VData " << std::get<1>(_val)[0] );
        auto lvl = this->RR().VPL( CommandBufferLimits::PBRStart, nullptr, 1.0f ); // mat->translucency()
        auto mat = this->SG().ML().get("tomato");
        VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ this->RR(), lvl, ShaderMaterial{S::SH, mat->Values()} }
                .p(this->generateGeometryVP(std::get<0>(_val))).n("_source->UUiD()").g(9200).build();

    });

    sg.nodeAddConnect( [this](NodeGraphConnectParamsSig _geom) {
        if ( auto as = std::get_if<GeomAssetSP>(&_geom); as != nullptr ) {
            (*as)->subscribeData(hierRenderObserver);
            (*as)->sendNotifyData("generateGeometryVP");
        } else if ( auto as = std::get_if<UIAssetSP>(&_geom); as != nullptr ) {
            (*as)->subscribeData(uiRenderObserver);
            (*as)->sendNotifyData("generateGeometryVP");
        } // else if ( auto as = std::get_if<CameraAssetSP>(&_geom); as != nullptr ) {
//        SG().CM().add( (*as)->Data() );
//    }
    });

    am = std::make_shared<AudioManagerOpenAL>();
}

//void RenderSceneGraph::addImpl( NodeVariants _geom ) {
//}

//void RenderSceneGraph::removeImpl( const UUID& _uuid ) {
//    auto removeF = [&](const UUID& _uuid) { rr.removeFromCL(_uuid ); };
//    if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
//        std::visit( [&](auto&& arg) { arg->visitHashRecF(removeF);}, it->second );
//    }
//}
//
//void RenderSceneGraph::cmdChangeTimeImpl( const std::vector<std::string>& _params ) {
//    SB().buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
//    RR().changeTime( SB().getSunPosition() );
//}
//
//void RenderSceneGraph::cmdloadObjectImpl( const std::vector<std::string>& _params ) {
//    Vector3f pos = Vector3f::ZERO;
//    Vector3f rot = Vector3f::ZERO;
//    std::string objName = _params[0];
//    if ( _params.size() == 4 || _params.size() == 7 ) {
//        pos = { std::stof(_params[1]), std::stof(_params[2]), std::stof(_params[3]) };
//    }
//    if ( _params.size() == 7 ) {
//        rot = { std::stof(_params[4]), std::stof(_params[5]), std::stof(_params[6]) };
//    }
////    ### I'm pretty positive Geom Files need to go through the load function of a dependent geom builder.
////    GF{*this, _params[0] }.build();
//}

void RenderSceneGraph::changeMaterialTagCallback( const std::vector<std::string>& _params ) {
//    std::shared_ptr<Material> mat = std::dynamic_pointer_cast<Material>(sg.ML().get(concatParams(_params, 1)));
//    rr.changeMaterialOnTagsCallback( { sg.getGeomType( _params[0] ), mat->Name() } );
}

//void RenderSceneGraph::changeMaterialTagImpl( const std::vector<std::string>& _params ) {
//    std::string keys = concatenate(" ", { _params.begin()+1, _params.end()} );
//    MB{ML(), concatParams(_params, 1)}.load(
//            std::bind( &RenderSceneGraph::changeMaterialTagCallback, this, _params ),
//            _params );
//}

void RenderSceneGraph::changeMaterialColorCallback( const std::vector<std::string>& _params ) {
    rr.changeMaterialColorOnTags( sg.getGeomType( _params[0] ), sg.CL().get(concatParams(_params, 1))->color );
}

Renderer& RenderSceneGraph::RR() { return rr; }

//void RenderSceneGraph::changeMaterialColorTagImpl( const std::vector<std::string>& _params ) {
//    ColorBuilder{cl, concatParams(_params, 1)}.load(std::bind( &RenderSceneGraph::changeMaterialColorCallback,
//                                                               this,
//                                                               _params), _params);
//}

//void RenderSceneGraph::cmdCreateGeometryImpl( const std::vector<std::string>& _params ) {
//
//    auto st = shapeTypeFromString( _params[0] );
//    if ( st != ShapeType::None) {
//        auto mat = ( _params.size() > 1 ) ? _params[1] : S::WHITE_PBR;
//        auto shd = ( _params.size() > 2 ) ? _params[2] : S::SH;
//        GB{*this, st }.n("ucarcamagnu").g(9200).m(shd,mat).build();
//    } else if ( toLower(_params[0]) == "text" && _params.size() > 1 ) {
//        Color4f col = _params.size() > 2 ? Vector4f::XTORGBA(_params[2]) : Color4f::BLACK;
//        UISB{*this, UIShapeType::Text3d, _params[1], 0.6f }.c(col).buildr();
//    }
//
//}
//
//void RenderSceneGraph::cmdRemoveGeometryImpl( const std::vector<std::string>& _params ) {
//    remove( _params[0] );
//}
//
//void RenderSceneGraph::updateImpl() {
//    am->update();
//}

std::shared_ptr<PosTexNorTanBinUV2Col3dStrip>
HierGeomRenderObserver::generateGeometryVP( const VData& __data ) {
    const VData* _data = &__data;
    if ( _data->numIndices() < 3 ) return nullptr;

    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[_data->numIndices()] );
    std::memcpy( _indices.get(), _data->Indices(), _data->numIndices() * sizeof( int32_t ));
    auto SOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( _data->numVerts(), PRIMITIVE_TRIANGLES,
                                                                   VFVertexAllocation::PreAllocate, _data->numIndices(),
                                                                   _indices );
    for ( int32_t t = 0; t < _data->numVerts(); t++ ) {
        SOAData->addVertex( _data->vertexAt( t ), _data->uvAt( t ), _data->uv2At( t ), _data->normalAt( t ),
                            _data->tangentAt( t ), _data->binormalAt( t ), _data->colorAt(t) );
    }
    return SOAData;
}

void HierGeomRenderObserver::notified( GeomAssetSP _source, const std::string& generator ) {
//    auto mat = _source->Data()->getMaterial();
//  ### MAT reinstate materials
    auto lvl = rr.VPL( CommandBufferLimits::PBRStart, _source->getLocalHierTransform(), 1.0f ); // mat->translucency()
    VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ rr, lvl, ShaderMaterial{S::SH} } //mat->getShaderName(), mat->Values()
            .p(generateGeometryVP(_source->Data()->vData())).n(_source->UUiD()).g(_source->GHType()).build();
}

HierGeomRenderObserver::HierGeomRenderObserver( Renderer& _rr ) : rr( _rr ) {}

void UIElementRenderObserver::notified( UIAssetSP _source, const std::string& generator ) {
//    auto mat = _source->Data()->getMaterial();
//  ### MAT reinstate materials
    auto renderBucketIndex = _source->Data()->RenderBucketIndex();
    auto vpList = rr.VPL( CommandBufferLimits::UIStart + renderBucketIndex, _source->getLocalHierTransform(), 1.0f );
    //mat->getOpacity()
    auto vs = _source->Data()->VertexList();
    VPBuilder<PosTex3dStrip>{rr,vpList, ShaderMaterial{S::SH}}. //mat->getShaderName(),mat->Values()
    p(vs).n(_source->UUiD()).build();
}

UIElementRenderObserver::UIElementRenderObserver( Renderer& _rr ) : rr( _rr ) {}
