//
// Created by Dado on 08/02/2018.
//

#include "render_scene_graph.h"
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/raw_image.h>
#include <core/node.hpp>
#include <core/geom.hpp>
#include <poly/resources/geom_builder.h>
#include <poly/resources/ui_shape_builder.h>
#include <graphics/renderer.h>
#include <graphics/vp_builder.hpp>
#include <graphics/audio/audio_manager_openal.hpp>
#include <render_scene_graph/scene_bridge.h>

RenderSceneGraph::RenderSceneGraph( Renderer& rr, SceneGraph& _sg ) : rr( rr ), sg(_sg) {

    sg.TL().connect( [this](const ResourceTransfer<RawImage>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<RawImage>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addTextureResource(_val);
    });

    sg.ML().connect( [this](const ResourceTransfer<Material>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<Material>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addMaterialResource(_val);
    });

    sg.VL().connect( [this](const ResourceTransfer<VData>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addVDataResource(_val);
    });

    sg.nodeAddConnect( [this](NodeGraphConnectParamsSig _geom) {
        LOGRS( "[SG-Node] Add " << _geom->Name() );
        auto dataRef = _geom->DataRef(0);
        auto vp = VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ this->RR(), dataRef.material, dataRef.vData}.n(_geom->UUiD()).build();
        this->RR().VPL( CommandBufferLimits::PBRStart, vp);
    });

    am = std::make_shared<AudioManagerOpenAL>();
}

void RenderSceneGraph::init() {
    // Set a fullscreen camera in case there's none
    addRig<CameraControlFly>( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f );
}

void RenderSceneGraph::updateInputs( const AggregatedInputData& _aid ) {
    for ( auto& [k,v] : mRigs ) {
        v->updateFromInputData( _aid );
    }
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

void RenderSceneGraph::addBoxToViewport( const std::string& _name, const SceneScreenBox& _box ) {
    if ( boxes.find(_name) != boxes.end() ) return;
    boxes[_name] = _box;
}

std::shared_ptr<CameraRig> RenderSceneGraph::getRig( const std::string& _name ) {
    return sg.CM().get(_name);
}

std::shared_ptr<Camera> RenderSceneGraph::getCamera( const std::string& _name ) {
    return sg.CM().get(_name)->getMainCamera();
}

void RenderSceneGraph::setViewportOnRig( std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport ) {
    rr.getTarget(_rig->Name())->getRig()->setViewport(_viewport);
}

void RenderSceneGraph::setViewportOnRig( const std::string& _rigName, const Rect2f& _viewport ) {
    rr.getTarget(_rigName)->getRig()->setViewport(_viewport);
}

void RenderSceneGraph::addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible ) {
//    if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
//        addBoxToViewport( _name,{ { _l, _r, _t, _b}, rlf->second } );
//        boxes[_name].setVisible( _bVisible );
//    }
}

void RenderSceneGraph::resizeCallback( const Vector2i& _resize ) {
    for ( auto& [k,v] : boxes ) {
        orBitWiseFlag( v.flags, BoxFlags::Resize );
        if ( getRig(k) ) {
            auto r = v.updateAndGetRect();
            rr.getTarget( k )->resize( r );
            sg.CM().get(k)->setViewport( r );
        }
    }
}

//void RenderSceneGraph::changeMaterialColorTagImpl( const std::vector<std::string>& _params ) {
//    ColorBuilder{cl, concatParams(_params, 1)}.load(std::bind( &RenderSceneGraph::changeMaterialColorCallback,
//                                                               this,
//                                                               _params), _params);
//}
//
//void RenderSceneGraph::cmdRemoveGeometryImpl( const std::vector<std::string>& _params ) {
//    remove( _params[0] );
//}
//
//void RenderSceneGraph::updateImpl() {
//    am->update();
//}
