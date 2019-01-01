//
// Created by Dado on 08/02/2018.
//

#include "render_scene_graph.h"
#include "core/image_builder.h"
#include "core/node.hpp"
#include "poly/geom_builder.h"
#include "poly/ui_shape_builder.h"

RenderSceneGraph::RenderSceneGraph( Renderer& rr, CommandQueue& cq, FontManager& fm ) : SceneGraph(cq, fm), rr( rr ), tl(rr.RIDM()) {
    hierRenderObserver = std::make_shared<HierGeomRenderObserver>(rr);
    uiRenderObserver = std::make_shared<UIElementRenderObserver>(rr);
    ml.TL(&tl);
}

void RenderSceneGraph::addImpl( GeomAssetSP _geom ) {
    _geom->subscribeData(hierRenderObserver);
    _geom->sendNotifyData("generateGeometryVP");
}

void RenderSceneGraph::addImpl( UIAssetSP _geom ) {
    _geom->subscribeData(uiRenderObserver);
    _geom->sendNotifyData("generateGeometryVP");
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

std::shared_ptr<PosTexNorTanBinUV2Col3dStrip>
HierGeomRenderObserver::generateGeometryVP( std::shared_ptr<GeomData> _data ) {
    if ( !_data ) return nullptr;
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
    auto lvl = rr.VPL( CommandBufferLimits::PBRStart, _source->getLocalHierTransform(), 1.0f );
    VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ rr,lvl }
            .p(generateGeometryVP( _source->Data())).m( _source->Data()->getMaterial()).n(_source->Hash()).g(_source->GHType()).build();
}

std::string UIElementRenderObserver::getShaderType( UIShapeType _st ) const {
    auto shaderName = S::TEXTURE_2D;
    switch ( _st ) {
        case UIShapeType::CameraFrustom2d:
        case UIShapeType::CameraFrustom3d:
            break;
        case UIShapeType::Rect2d:
        case UIShapeType::Rect3d:
            shaderName = _st == UIShapeType::Rect2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Line2d:
        case UIShapeType::Line3d:
            shaderName = _st == UIShapeType::Line2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Arrow2d:
        case UIShapeType::Arrow3d:
            shaderName = _st == UIShapeType::Arrow2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Polygon2d:
        case UIShapeType::Polygon3d:
            shaderName = _st == UIShapeType::Polygon2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Text2d:
        case UIShapeType::Text3d:
            shaderName = _st == UIShapeType::Text2d ? S::FONT_2D : S::FONT;
            break;
        case UIShapeType::Separator2d:
        case UIShapeType::Separator3d:
            shaderName = _st == UIShapeType::Separator2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
    }

    return shaderName;
}

void UIElementRenderObserver::notified( UIAssetSP _source, const std::string& generator ) {
    auto color = _source->Data()->Color();
    auto renderBucketIndex = _source->Data()->RenderBucketIndex();
    auto vpList = rr.VPL( CommandBufferLimits::UIStart + renderBucketIndex, _source->getLocalHierTransform(), color.w() );
    auto shaderName = getShaderType( _source->Data()->ShapeType() );
    auto vs = _source->Data()->VertexList();
    VPBuilder<PosTex3dStrip>{rr,vpList}.p(vs).s(shaderName).c(color).n(_source->Hash()).build();
}
