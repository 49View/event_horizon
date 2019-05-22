//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include <core/lightmap_exchange_format.h>
#include <core/node.hpp>
#include <core/raw_image.h>
#include <core/camera_rig.hpp>
#include <core/resources/profile.hpp>
#include <core/TTF.h>
#include <core/geom.hpp>
#include <core/names.hpp>
#include <core/resources/resource_builder.hpp>
#include <poly/resources/ui_shape_builder.h>
#include <core/resources/material.h>
#include <core/file_manager.h>
#include <poly/converters/gltf2/gltf2.h>
#include <poly/baking/xatlas_client.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <poly/converters/obj/tiny_obj_loader.h>

GenericSceneCallback           SceneGraph::genericSceneCallback         ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackVData        ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackRawImage     ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterial     ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackFont         ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackProfile      ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterialColor;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackCameraRig    ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackGeom         ;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackComposite    ;

GeomSP SceneGraph::addNode( const ResourceRef& _hash ) {
    auto cloned = GM().clone( _hash );
    nodeAddSignal(cloned);
    nodes.emplace( cloned->UUiD(), cloned );
    return cloned;//->UUiD();
}

void SceneGraph::removeNode( const UUID& _uuid ) {
    if ( auto it = nodes.find(_uuid); it != nodes.end() ) {
        // Remove all child
//        removeImpl(_uuid);
        nodes.erase( it );
    }
}

GeomSP SceneGraph::getNode(  const UUID& _uuid ) {
    if ( auto it = nodes.find(_uuid); it != nodes.end() ) {
        return it->second;
    }
    return nullptr;
}

void SceneGraph::update() {

    for ( auto& [k, v] : genericSceneCallback ) {

        if ( k == ResourceGroup::Image ) {
            B<IB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else if ( k == ResourceGroup::Font ) {
            B<FB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else if ( k == ResourceGroup::Profile ) {
            B<PB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else if ( k == ResourceGroup::Color ) {
            B<MCB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else if ( k == ResourceGroup::Material ) {
            B<MB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else if ( k == ResourceGroup::Geom ) {
            GLTF2Service::load( *this, std::get<2>(v) );
//            for ( auto& scene : glt ) {
//                scene->visit( [this]( std::shared_ptr<GeomSceneArtifact> _geom ) {
//                    if ( !_geom->empty() ) {
//                        B<MB>( "urca" ).addIM( _geom->Data().materialSP );
//                    }
//                });
//            }
//            std::make_shared<GLTF2>( std::get<1>(v), std::get<0>(v) );
//            B<MB>( std::get<0>(v) ).publishAndAdd( std::get<1>(v) );
        } else {
            LOGRS("{" << k << "} Resource not supported yet in callback updating");
            ASSERT(0);
        }
//        auto tr = sceneEntityFilesCallbacks[T::Prefix()];
//        B<T>(std::get<0>(tr)).publishAndAdd( std::get<1>(tr) );
//        c( this );
    }
    genericSceneCallback.clear();

    for ( const auto& res : resourceCallbackVData        ) {addVData        ( res.key, VData        {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackRawImage     ) {addRawImage     ( res.key, RawImage     {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackMaterial     ) {addMaterial     ( res.key, Material     {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackFont         ) {addFont         ( res.key, Font         {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackProfile      ) {addProfile      ( res.key, Profile      {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackMaterialColor) {addMaterialColor( res.key, MaterialColor{res.data}, res.ccf ); }
//    for ( const auto& res : resourceCallbackCameraRig    ) {addCameraRig    ( res.key, CameraRig    {res.data}, res.ccf ); }
//    for ( const auto& res : resourceCallbackGeom         ) {addGeom         ( res.key, Geom         {res.data}, res.ccf ); }
    for ( const auto& res : resourceCallbackComposite    ) {addResources( res.data, res.ccf ); }

    resourceCallbackVData        .clear();
    resourceCallbackRawImage     .clear();
    resourceCallbackMaterial     .clear();
    resourceCallbackFont         .clear();
    resourceCallbackProfile      .clear();
    resourceCallbackMaterialColor.clear();
    resourceCallbackCameraRig    .clear();
    resourceCallbackGeom         .clear();
    resourceCallbackComposite    .clear();

    VL().update();
    TL().update();
    PL().update();
    ML().update();
    CL().update();
    CM().update();
    FM().update();
    MC().update();
    GM().update();

    for ( auto& [k,v] : nodes ) {
        v->updateAnim();
//        std::visit( lambdaUpdateAnimVisitor, v );
    }
}

void SceneGraph::cmdChangeMaterialTag( const std::vector<std::string>& _params ) {
//    changeMaterialTagImpl( _params );
}

void SceneGraph::cmdChangeMaterialColorTag( const std::vector<std::string>& _params ) {
//    changeMaterialColorTagImpl( _params );
}

void SceneGraph::cmdCreateGeometry( const std::vector<std::string>& _params ) {
    auto st = shapeTypeFromString( _params[0] );
    if ( st != ShapeType::None) {
        auto mat = ( _params.size() > 1 ) ? _params[1] : S::WHITE_PBR;
//        GB{ *this, st }.m(mat).n("ucarcamagnu").g(9200).build();
//        GB{ *this, Rect2f::IDENTITY, 0.0f }.build();
//        std::vector<V3f> vlist { V3f::ZERO, V3f{0.8f, 0.0f, 0.0f}, V3f{0.2f, 0.0f, 0.9f } };
//        GB{ *this, vlist, 0.2f }.build();
//        auto pr = std::make_shared<Profile>();
//        pr->createWire(0.1f, 6);
//        auto prId = B<PB>("ProfileWire").addIM(pr);
//        auto gref = GB{ *this, GeomBuilderType::follower, prId, vlist }.c(Color4f::RED).build();
//        addNode( gref );
    } else if ( toLower(_params[0]) == "text" && _params.size() > 1 ) {
//        Color4f col = _params.size() > 2 ? Vector4f::XTORGBA(_params[2]) : Color4f::BLACK;
        UISB{*this, UIShapeType::Text3d, _params[1], 0.6f }.buildr();
    }
}

void SceneGraph::cmdRemoveGeometry( const std::vector<std::string>& _params ) {
//    cmdRemoveGeometryImpl( _params );
}

void SceneGraph::cmdLoadObject( const std::vector<std::string>& _params ) {
//    cmdloadObjectImpl( _params );
}

void SceneGraph::cmdCalcLightmaps( const std::vector<std::string>& _params ) {
//    cmdCalcLightmapsImpl( _params );
}

void SceneGraph::cmdChangeTime( const std::vector<std::string>& _params ) {
//    cmdChangeTimeImpl( _params );
}

void SceneGraph::cmdReloadShaders( const std::vector<std::string>& _params ) {
//    cmdChangeTimeImpl( _params );
}

SceneGraph::SceneGraph( CommandQueue& cq,
                        VDataManager& _vl,
                        ImageManager& _tl,
                        ProfileManager& _pl,
                        MaterialManager& _ml,
                        ColorManager& _cl,
                        FontManager& _fm,
                        CameraManager& _cm,
                        GeomManager& _gm ) : vl(_vl), tl(_tl), pl(_pl), ml(_ml), cl(_cl), fm(_fm), cm(_cm), gm(_gm) {

    hcs = std::make_shared<CommandScriptSceneGraph>(*this);
    cq.registerCommandScript(hcs);
    mapGeomType(0, "none");
    mapGeomType(1, "generic");
}

size_t SceneGraph::countGeoms() const {
    return nodes.size();
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find(_key); ret != geomTypeMap.end() ) {
        return ret->second;
    }
    try {
        return static_cast<uint64_t>(std::stoi( _key ));
    }
    catch(...) {
        return 0;
    }
    return 0;
}

NodeGraphContainer& SceneGraph::Nodes() {
    return nodes;
}

bool SceneGraph::rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback ) {

    bool ret = false;

    for ( const auto& [k, v] : nodes ) {
        AABB box = AABB::INVALID;
        UUID uuid{};
//        ### REF reimplement box and UUID
//        box = v->BBox3d();
//        uuid = (*as)->UUiD();
        float tn = 0.0f;
        float tf = std::numeric_limits<float>::max();
        auto ldir = normalize( _far - _near );
        if ( box.intersectLine( _near, ldir, tn, tf) ) {
            _callback( v, tn );
            ret = true;
            break;
        }
    }

    return ret;
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition("change material", std::bind(&SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("paint", std::bind(&SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("add", std::bind(&SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("remove", std::bind(&SceneGraph::cmdRemoveGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("load object", std::bind(&SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1));
    addCommandDefinition("lightmaps", std::bind(&SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1));
    addCommandDefinition("change time", std::bind(&SceneGraph::cmdChangeTime, &_hm, std::placeholders::_1 ));
    addCommandDefinition("reload shaders", std::bind(&SceneGraph::cmdReloadShaders, &_hm, std::placeholders::_1 ));
}

void SceneGraph::init() {
    B<IB>( S::WHITE     ).addIM( RawImage::WHITE4x4()      );
    B<IB>( S::BLACK     ).addIM( RawImage::BLACK_ARGB4x4() );
    B<IB>( S::NORMAL    ).addIM( RawImage::NORMAL4x4()     );
    B<IB>( S::DEBUG_UV  ).addIM( RawImage::DEBUG_UV()      );

    B<MB>( S::WHITE_PBR ).addIM( Material{S::SH} );
//    B<MB>( "tomato" ).load();
//    B<FB>( S::DEFAULT_FONT ).addIM( Font{Poppins_Medium_ttf, Poppins_Medium_ttf_len} );
//    B<FB>( S::DEFAULT_FONT ).addIM( Font{FM::readLocalFileC("/Users/Dado/Downloads/Banner5_ForSending/AkkuratFont/Akkurat-Light.ttf")} );
    B<CB>( Name::Foxtrot ).addIM( CameraRig{Name::Foxtrot} );
}

ResourceRef SceneGraph::addVData         ( const ResourceRef& _key, const VData        & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<VB> (_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addRawImage      ( const ResourceRef& _key, const RawImage     & _res, HttpDeferredResouceCallbackFunction _ccf ) {
    B<IB> (_key).addDF( _res, _ccf );
    return _key;
}
ResourceRef SceneGraph::addMaterial      ( const ResourceRef& _key, const Material     & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<MB> (_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addFont          ( const ResourceRef& _key, const Font         & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<FB> (_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addProfile       ( const ResourceRef& _key, const Profile      & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<PB> (_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addMaterialColor ( const ResourceRef& _key, const MaterialColor& _res, HttpDeferredResouceCallbackFunction _ccf ) { B<MCB>(_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addCameraRig     ( const ResourceRef& _key, const CameraRig    & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<CB> (_key).addDF( _res, _ccf ); return _key; }
ResourceRef SceneGraph::addGeom          ( const ResourceRef& _key, const Geom         & _res, HttpDeferredResouceCallbackFunction _ccf ) { B<GRB>(_key).addDF( _res, _ccf ); return _key; }

void SceneGraph::addResources( const SerializableContainer& _data, HttpDeferredResouceCallbackFunction _ccf ) {

    auto fs = tarUtil::untar(_data);
    ASSERT( fs.find(ResourceCatalog::Key) != fs.end() );
    auto dict = deserializeArray<ResourceTarDict>( fs[ResourceCatalog::Key] );

    std::sort( dict.begin(), dict.end(), []( const auto& a, const auto& b ) -> bool {
        return resourcePriority( a.group ) < resourcePriority( b.group );
    } );

    for ( const auto& rd : dict ) {
        if ( rd.group == ResourceGroup::Image ) {
            B<IB>( rd.filename ).make( fs[rd.filename], rd.hash );
        } else if ( rd.group == ResourceGroup::Font ) {
            B<FB>( rd.filename ).make( fs[rd.filename], rd.hash );
        } else if ( rd.group == ResourceGroup::Profile ) {
            B<PB>( rd.filename ).make( fs[rd.filename], rd.hash );
        } else if ( rd.group == ResourceGroup::Color ) {
            B<MCB>( rd.filename ).make( fs[rd.filename], rd.hash );
        } else if ( rd.group == ResourceGroup::Material ) {
            B<MB>( rd.filename ).make( fs[rd.filename], rd.hash );
        } else {
            LOGRS("{" << rd.group << "} Resource not supported yet in dependency unpacking");
            ASSERT(0);
        }
    }

    if ( _ccf) _ccf();
}

GeomSP SceneGraph::GC() {
    auto ret = std::make_shared<Geom>();
    ret->Name( B<GRB>( ret->UUiD() ).addIM( ret ) );
    return ret;
}

void SceneGraph::GC( const GeomSP& _geom ) {
    auto ref = GM().getHash( _geom->Name() );
    if ( !ref.empty() ) addNode( ref );
    for (const auto& c : _geom->Children() ) {
        GC( c );
    }
}

ResourceRef SceneGraph::GBMatInternal( CResourceRef _matref, const C4f& _color ) {
    auto matRef = ML().getHash(_matref);
    if ( _color != C4f::WHITE ) {
        auto matClone = EF::clone( ML().get(matRef) );
        matClone->setDiffuseColor( _color.xyz() );
        matClone->setOpacity( _color.w() );
        matRef = B<MB>(_matref+_color.toString()).addIM( matClone );
    }
    return matRef;
}

void writeFace( size_t base, size_t i1, size_t i2,size_t i3, std::ostringstream& ssf ) {
    size_t pi1 = base + i1;
    size_t pi2 = base + i2;
    size_t pi3 = base + i3;
    ssf << "f " << pi1 << "/" << pi1   << "/" << pi1  << " ";
    ssf         << pi2 << "/" << pi2 << "/" << pi2 << " ";
    ssf         << pi3 << "/" << pi3 << "/" << pi3;
    ssf << std::endl;
}

void SceneGraph::chartMeshes( scene_t& scene ) const {
    std::ostringstream ss;
    std::ostringstream ssf;
    size_t totalVerts = 1;

    for ( const auto& gg : gm.list() ) {
        if ( !gg->empty() ) {
            auto mat = gg->getLocalHierTransform();
            auto vData = vl.get(gg->Data(0).vData);
            for ( size_t t = 0; t < vData->numVerts(); t++ ) {
                auto v = vData->vertexAt(t);
                v = mat->transform(v);
                ss << v.toStringObj("v");
                auto n = vData->normalAt(t);
                n = mat->transform(n);
                n = normalize(n);
                ss << n.toStringObj("vn");
                ss << vData->uvAt(t).toStringObj("vt");
            }
            for ( size_t t = 0; t < vData->numIndices(); t+=3 ) {
                writeFace( totalVerts,
                           vData->getVIndices()[t],
                           vData->getVIndices()[t+1],
                           vData->getVIndices()[t+2],
                           ssf );
            }
            totalVerts += vData->numIndices();
        }
    }

    ss << ssf.str();
    std::istringstream ssi(ss.str());
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string basePath;
    tinyobj::MaterialFileReader matFileReader(basePath);
    std::string err;
    if (!tinyobj::LoadObj(shapes, materials, err, ssi, matFileReader, tinyobj::triangulation)) {
        LOGR("Error: %s\n", err.c_str());
    }
    xatlasParametrize( shapes, &scene );

//    FM::writeLocalFile("house.obj", ss.str() );
}
