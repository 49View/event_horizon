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
#include <core/image_util.h>
#include <core/geom.hpp>
#include <core/camera.h>
#include <core/names.hpp>
#include <core/resources/resource_builder.hpp>
#include <core/resources/material.h>
#include <core/resources/ui_container.hpp>
#include <core/file_manager.h>
#include <poly/converters/gltf2/gltf2.h>
#include <poly/baking/xatlas_client.hpp>
#include <poly/scene_events.h>

#define TINYOBJLOADER_IMPLEMENTATION

#include <poly/converters/obj/tiny_obj_loader.h>

GenericSceneCallback           SceneGraph::genericSceneCallback;
EventSceneCallback             SceneGraph::eventSceneCallback;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackVData;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackRawImage;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterial;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackFont;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackProfile;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterialColor;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackCameraRig;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackGeom;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackUI;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackComposite;

void SceneGraph::addNode( GeomSP _node ) {
    nodeAddSignal( _node );
    nodes.emplace( _node->UUiD(), _node );
    for ( const auto& c : _node->Children()) {
        addNode( c );
    }
}

void SceneGraph::addNode( const UUID& _uuid ) {
    addNode( get<Geom>( _uuid ));
}

void SceneGraph::removeNode( const UUID& _uuid ) {
    if ( auto it = nodes.find( _uuid ); it != nodes.end()) {
        nodes.erase( it );
    }
}

void SceneGraph::removeNode( GeomSP _node ) {
    nodeRemoveSignal( _node );
    if ( _node ) {
        for ( const auto& c : _node->Children()) {
            removeNode( c );
        }
        removeNode( _node->UUiD());
    }
}

GeomSP SceneGraph::getNode( const UUID& _uuid ) {
    if ( auto it = nodes.find( _uuid ); it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Camera> SceneGraph::DC() {
    return CM().get( Name::Foxtrot )->getCamera();
}

JSONDATA( MatGeomSerData, mrefs )

    std::unordered_map<std::string, MaterialThumbnail> mrefs;
};

JSONDATA( LoadFinishData, flag )

    bool flag = true;
};

void MaterialThumbnail::setThumbnailFor( const std::string& _textureType ) {
    auto dt = sg->get<RawImage>( getTexture( _textureType ));
    constexpr int ts = 32;

    if ( dt ) {
        bool isBigEnough = dt->width > ts || dt->height > ts;
        thumbValues[_textureType] = isBigEnough ?
                                    imageUtil::rawResizeToPng64gzip( dt, ts, ts )
                                                : imageUtil::rawToPng64gzip( *dt );
    }

}

MaterialThumbnail::MaterialThumbnail( SceneGraph *_sg, const Material& _mat ) : Material( _mat ), sg( _sg ) {
    setThumbnailFor( UniformNames::diffuseTexture );
    setThumbnailFor( UniformNames::normalTexture );
    setThumbnailFor( UniformNames::metallicTexture );
    setThumbnailFor( UniformNames::roughnessTexture );
    setThumbnailFor( UniformNames::aoTexture );
    setThumbnailFor( UniformNames::opacityTexture );
}

void SceneGraph::materialsForGeomSocketMessage() {
    MatGeomSerData matSet{};
    for ( const auto&[k, node] : Nodes()) {
        for ( const auto& data : node->DataV()) {
            auto mat = get<Material>( data.material );
            matSet.mrefs.emplace( mat->Key(), MaterialThumbnail( this, *mat ));
        }
    }
    Socket::send( "materialsForGeom", matSet );
}

void SceneGraph::replaceMaterialOnNodes( const std::string& _key ) {
    LOGRS( "OldMaterialRef: " << signalValueMap["source_material_id"] );
    for ( auto&[k, node] : Nodes()) {
        for ( auto& data : node->DataVRef()) {
            auto mat = get<Material>( data.material );
            if ( mat->Key() == signalValueMap["source_material_id"] ) {
                data.material = _key;
            }
        }
    }
    materialsForGeomSocketMessage();
    replaceMaterialSignal( signalValueMap["source_material_id"], _key );
}

void SceneGraph::publishAndAddCallback() {
    for ( auto&[k, v] : genericSceneCallback ) {
        if ( k == ResourceGroup::Image ) {
            B<IB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else if ( k == ResourceGroup::Font ) {
            B<FB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else if ( k == ResourceGroup::Profile ) {
            B<PB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else if ( k == ResourceGroup::Color ) {
            B<MCB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else if ( k == ResourceGroup::Material ) {
            B<MB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else if ( k == ResourceGroup::Geom ) {
            B<GRB>( std::get<0>( v )).publishAndAdd( std::get<1>( v ));
        } else {
            LOGRS( "{" << k << "} Resource not supported yet in callback updating" );
            ASSERT( 0 );
        }
    }
    genericSceneCallback.clear();
}

void SceneGraph::clearFromRealTimeCallbacks() {
    removeNode( !nodes.empty() ? nodes.begin()->second : nullptr );
    GM().clear();
    Nodes().clear();
};

void SceneGraph::realTimeCallbacks() {
    for ( auto&[k, doc] : eventSceneCallback ) {

        if ( k == SceneEvents::ReloadLuaScript ) {
            runLUAScriptSignal( doc["data"].GetString());
        } else if ( k == SceneEvents::UpdateEntity ) {
            clearFromRealTimeCallbacks();
            auto entityGroup = doc["data"]["group"].GetString();

            if ( entityGroup == ResourceGroup::UI ) {
                um.clear();
                auto rref = addUIIM( UUIDGen::make(), UIContainer{ doc["data"]["data"] } );
                nodeFullScreenUIContainerSignal( rref );
            }
        } else if ( k == SceneEvents::AddPlaceHolderEntity ) {
            clearFromRealTimeCallbacks();
            auto entityGroup = doc["data"]["group"].GetString();

            LOGRS( "EntityGroup of AddPlaceHolder:" << entityGroup );
            if ( entityGroup == ResourceGroup::UI ) {
                um.clear();
                auto rref = addUIIM( UUIDGen::make(), UIContainer::placeHolder());
                nodeFullScreenUIContainerSignal( rref );
            }
        } else if ( k == SceneEvents::LoadGeomAndReset ) {
            auto v0 = getFileName( doc["data"]["entity_id"].GetString());
            auto vHash = getFileName( doc["data"]["hash"].GetString());

            clearFromRealTimeCallbacks();
            Http::clearRequestCache();

            auto entityGroup = doc["data"]["group"].GetString();

            if ( entityGroup == ResourceGroup::Geom ) {
                load<Geom>( v0, [this]( HttpResouceCBSign key ) {
                    auto geom = GB<GT::Asset>( key, GT::Tag( 1001 ));
                    geom->updateTransform( V3f::ZERO, Quaternion{ (float) M_PI, V3f::UP_AXIS }, V3f::ONE );
                    DC()->center( geom->BBox3dCopy(), CameraCenterAngle::HalfwayOpposite );
                    materialsForGeomSocketMessage();
                } );
            } else if ( entityGroup == ResourceGroup::Material ) {
                load<Material>( v0, [this, vHash]( HttpResouceCBSign key ) {
                    auto geom = GB<GT::Shape>( ShapeType::Sphere, GT::Tag( 1001 ), GT::M( vHash ));
                    DC()->center( geom->BBox3dCopy(), CameraCenterAngle::Back );
                    Socket::send( "wasmClientFinishedLoadingData", LoadFinishData{} );
                } );
            } else if ( entityGroup == ResourceGroup::Image ) {
                load<RawImage>( v0, [this, vHash]( HttpResouceCBSign key ) {
                    nodeFullScreenImageSignal( key );
                } );
            } else if ( entityGroup == ResourceGroup::Font ) {
                load<Font>( v0, [this, vHash]( HttpResouceCBSign key ) {
                    nodeFullScreenFontSonnetSignal( key );
                } );
            } else if ( entityGroup == ResourceGroup::UI ) {
                load<UIContainer>( v0, [this, vHash]( HttpResouceCBSign key ) {
                    nodeFullScreenUIContainerSignal( key );
                } );
            }
        } else if ( k == SceneEvents::ReplaceMaterialOnCurrentObject ) {
            auto matId = getFileName( doc["data"]["mat_id"].GetString());
            auto objId = getFileName( doc["data"]["entity_id"].GetString());
            signalValueMap["source_material_id"] = std::string( doc["data"]["source_id"].GetString());
            load<Material>( matId, [&]( HttpResouceCBSign key ) {
                replaceMaterialOnNodes( key );
            } );
        } else if ( k == SceneEvents::ChangeMaterialProperty ) {
            if ( doc["data"]["value_type"].GetString() == std::string( "hexcolor" )) {
                auto value = C4f::XTORGBA( doc["data"]["value_str"].GetString()).xyz();
                changeMaterialPropertyV3fSignal( doc["data"]["property_id"].GetString(),
                                                 doc["data"]["mat_id"].GetString(), value );
            } else if ( doc["data"]["value_type"].GetString() == std::string( "float100" )) {
                float value = std::stof( doc["data"]["value_str"].GetString()) / 100.0f;
                changeMaterialPropertyFloatSignal( doc["data"]["property_id"].GetString(),
                                                   doc["data"]["mat_id"].GetString(), value );
            } else if ( doc["data"]["value_type"].GetString() == std::string( "string" )) {
                std::string value = doc["data"]["value_str"].GetString();
                std::string pid = doc["data"]["property_id"].GetString();
                std::string matid = doc["data"]["mat_id"].GetString();
                acquire<RawImage>( value, [this, pid, matid]( HttpResouceCBSign key ) {
                    changeMaterialPropertyStringSignal( pid,
                                                        matid, key );
                } );
            }
        }
    }
    eventSceneCallback.clear();
}

void SceneGraph::loadCallbacks() {
    loadResourceCallback<VData, VB>( resourceCallbackVData );
    loadResourceCallback<RawImage, IB>( resourceCallbackRawImage );
    loadResourceCallback<Font, FB>( resourceCallbackFont );
    loadResourceCallback<Profile, PB>( resourceCallbackProfile );
    loadResourceCallback<UIContainer, UIB>( resourceCallbackUI );
    loadResourceCallback<MaterialColor, MCB>( resourceCallbackMaterialColor );
    loadResourceCallbackWithKey<Material, MB>( resourceCallbackMaterial );
    loadResourceCallbackWithLoader<Geom, GRB>( resourceCallbackGeom, GLTF2Service::load );
    loadResourceCompositeCallback( resourceCallbackComposite );
}

void SceneGraph::update() {

    static bool firstFrameEver = true;

    if ( firstFrameEver ) {
        firstFrameEver = false;
        return;
    }

    // NDDado, these are mainly dealt with the react portal now, might become obsolete soon
    publishAndAddCallback();
    realTimeCallbacks();
    loadCallbacks();

    VL().update();
    TL().update();
    PL().update();
    ML().update();
    CL().update();
    CM().update();
    FM().update();
    MC().update();
    GM().update();
    UM().update();

    for ( auto&[k, v] : nodes ) {
        v->updateAnim();
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
    if ( st != ShapeType::None ) {
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
                        GeomManager& _gm,
                        UIManager& _um,
                        LightManager& _ll ) : vl( _vl ), tl( _tl ), pl( _pl ), ml( _ml ), cl( _cl ), fm( _fm ),
                                              cm( _cm ), gm( _gm ), um( _um ), ll( _ll ) {

    hcs = std::make_shared<CommandScriptSceneGraph>( *this );
    cq.registerCommandScript( hcs );
    mapGeomType( 0, "none" );
    mapGeomType( 1, "generic" );
}

size_t SceneGraph::countGeoms() const {
    return nodes.size();
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find( _key ); ret != geomTypeMap.end()) {
        return ret->second;
    }
    try {
        return static_cast<uint64_t>(std::stoi( _key ));
    }
    catch ( ... ) {
        return 0;
    }
    return 0;
}

NodeGraphContainer& SceneGraph::Nodes() {
    return nodes;
}

bool SceneGraph::rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback ) {

    bool ret = false;

    for ( const auto&[k, v] : nodes ) {
        AABB box = AABB::INVALID;
        UUID uuid{};
//        ### REF reimplement box and UUID
//        box = v->BBox3d();
//        uuid = (*as)->UUiD();
        float tn = 0.0f;
        float tf = std::numeric_limits<float>::max();
        auto ldir = normalize( _far - _near );
        if ( box.intersectLine( _near, ldir, tn, tf )) {
            _callback( v, tn );
            ret = true;
            break;
        }
    }

    return ret;
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition( "change material",
                          std::bind( &SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "paint", std::bind( &SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "add", std::bind( &SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "remove", std::bind( &SceneGraph::cmdRemoveGeometry, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "load object", std::bind( &SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "lightmaps", std::bind( &SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "change time", std::bind( &SceneGraph::cmdChangeTime, &_hm, std::placeholders::_1 ));
    addCommandDefinition( "reload shaders", std::bind( &SceneGraph::cmdReloadShaders, &_hm, std::placeholders::_1 ));
}

void SceneGraph::init() {
    B<IB>( S::WHITE ).addIM( RawImage::WHITE4x4());
    B<IB>( S::BLACK ).addIM( RawImage::BLACK_ARGB4x4());
    B<IB>( S::NORMAL ).addIM( RawImage::NORMAL4x4());
    B<IB>( S::NOISE4x4 ).addIM( RawImage::NOISE4x4());
    B<IB>( S::DEBUG_UV ).addIM( RawImage::DEBUG_UV());
    B<IB>( S::LUT_3D_TEST ).addIM( RawImage::LUT_3D_TEST());

    B<MB>( S::WHITE_PBR ).addIM( Material{ S::SH } );
    Profile squared;
    squared.createRect( Rect2f::IDENTITY_CENTERED );
    B<PB>( S::SQUARE ).addIM( squared );
    B<CB>( Name::Foxtrot ).addIM( CameraRig{ Name::Foxtrot } );
}

ResourceRef SceneGraph::addVData( const ResourceRef& _key, const VData& _res, HttpResouceCB _ccf ) {
    B<VB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addRawImage( const ResourceRef& _key, const RawImage& _res, HttpResouceCB _ccf ) {
    B<IB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addMaterial( const ResourceRef& _key, const Material& _res, HttpResouceCB _ccf ) {
    B<MB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addMaterialIM( const ResourceRef& _key, const Material& _res ) {
    B<MB>( _key ).addIM( _res );
    return _key;
}

ResourceRef SceneGraph::addFont( const ResourceRef& _key, const Font& _res, HttpResouceCB _ccf ) {
    B<FB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addProfile( const ResourceRef& _key, const Profile& _res, HttpResouceCB _ccf ) {
    B<PB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addMaterialColor( const ResourceRef& _key, const MaterialColor& _res, HttpResouceCB _ccf ) {
    B<MCB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addCameraRig( const ResourceRef& _key, const CameraRig& _res, HttpResouceCB _ccf ) {
    B<CB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addGeom( const ResourceRef& _key, GeomSP _res, HttpResouceCB _ccf ) {
    B<GRB>( _key ).addIM( _res );
    if ( _ccf ) _ccf( _key );
    return _key;
}

ResourceRef SceneGraph::addLight( const ResourceRef& _key, const Light& _res, HttpResouceCB _ccf ) {
    B<LB>( _key ).addIM( _res );
    if ( _ccf ) _ccf( _key );
    return _key;
}

ResourceRef SceneGraph::addUI( const ResourceRef& _key, const UIContainer& _res, HttpResouceCB _ccf ) {
    B<UIB>( _key ).addDF( _res, _ccf );
    return _key;
}

ResourceRef SceneGraph::addUIIM( const ResourceRef& _key, const UIContainer& _res ) {
    B<UIB>( _key ).addIM( _res );
    return _key;
}

void SceneGraph::addResources( CResourceRef _key, const SerializableContainer& _data, HttpResouceCB _ccf ) {

    auto fs = tarUtil::untar( _data );
    ASSERT( fs.find( ResourceCatalog::Key ) != fs.end());
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
            auto mat = B<MB>( rd.filename ).make( fs[rd.filename], rd.hash, _key );
            mat->Key( _key );
        } else {
            LOGRS( "{" << rd.group << "} Resource not supported yet in dependency unpacking" );
            ASSERT( 0 );
        }
    }

    if ( _ccf ) _ccf( _key );
}

ResourceRef SceneGraph::GBMatInternal( CResourceRef _matref, const C4f& _color ) {
    auto matRef = ML().getHash( _matref );
    if ( matRef.empty()) {
        matRef = ML().getHash( S::WHITE_PBR );
    }
    if ( _color != C4f::WHITE ) {
        auto matClone = EF::clone( ML().get( matRef ));
        matClone->setDiffuseColor( _color.xyz());
        matClone->setOpacity( _color.w());
        matRef = B<MB>( _matref + _color.toString()).addIM( matClone );
    }
    return matRef;
}

void writeFace( size_t base, size_t i1, size_t i2, size_t i3, std::ostringstream& ssf ) {
    size_t pi1 = base + i1;
    size_t pi2 = base + i2;
    size_t pi3 = base + i3;
    ssf << "f " << pi1 << "/" << pi1 << "/" << pi1 << " ";
    ssf << pi2 << "/" << pi2 << "/" << pi2 << " ";
    ssf << pi3 << "/" << pi3 << "/" << pi3;
    ssf << std::endl;
}

void SceneGraph::chartMeshes( scene_t& scene ) {
    std::ostringstream ss;
    std::ostringstream ssf;
    size_t totalVerts = 1;

    std::vector<VertexOffsetScene> unchart;

    size_t currUnchartOffset = 0;
    for ( const auto&[k, gg] : nodes ) {
        if ( !gg->empty()) {
            auto mat = gg->getLocalHierTransform();
            auto vData = vl.get( gg->Data( 0 ).vData );
            unchart.emplace_back( gg->UUiD(), currUnchartOffset, vData->numVerts(), currUnchartOffset,
                                  vData->numVerts());
            currUnchartOffset += vData->numVerts();
            for ( size_t t = 0; t < vData->numVerts(); t++ ) {
                auto v = vData->vertexAt( t );
                v = mat->transform( v );
                ss << v.toStringObj( "v" );
                auto n = vData->normalAt( t );
                n = mat->transform( n );
                n = normalize( n );
                ss << n.toStringObj( "vn" );
                ss << vData->uvAt( t ).toStringObj( "vt" );
            }
            for ( size_t t = 0; t < vData->numIndices(); t += 3 ) {
                writeFace( totalVerts,
                           vData->getVIndices()[t],
                           vData->getVIndices()[t + 1],
                           vData->getVIndices()[t + 2],
                           ssf );
            }
            totalVerts += vData->numIndices();
        }
    }

    ss << ssf.str();
    LOGRS( ss.str());
    std::istringstream ssi( ss.str());
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string basePath;
    tinyobj::MaterialFileReader matFileReader( basePath );
    std::string err;
    if ( !tinyobj::LoadObj( shapes, materials, err, ssi, matFileReader, 0 )) {
        LOGR( "Error: %s\n", err.c_str());
    }

    xatlasParametrize( shapes, &scene );
//    FM::writeLocalFile("house.obj", ss.str() );

}

void SceneGraph::chartMeshes2( scene_t& scene ) {
    xatlasParametrize( *this, nodes, &scene );
}

void SceneGraph::loadVData( std::string _names, HttpResouceCB _ccf ) {
    B<VB>( _names ).load( _ccf );
}

void SceneGraph::loadRawImage( std::string _names, HttpResouceCB _ccf ) {
    B<IB>( _names ).load( _ccf );
}

void SceneGraph::loadMaterial( std::string _names, HttpResouceCB _ccf ) {
    B<MB>( _names ).load( _ccf );
}

void SceneGraph::loadFont( std::string _names, HttpResouceCB _ccf ) {
    B<FB>( _names ).load( _ccf );
}

void SceneGraph::loadProfile( std::string _names, HttpResouceCB _ccf ) {
    B<PB>( _names ).load( _ccf );
}

void SceneGraph::loadMaterialColor( std::string _names, HttpResouceCB _ccf ) {
    B<MCB>( _names ).load( _ccf );
}

void SceneGraph::loadCameraRig( std::string _names, HttpResouceCB _ccf ) {
    B<CB>( _names ).load( _ccf );
}

void SceneGraph::loadUI( std::string _names, HttpResouceCB _ccf ) {
    B<UIB>( _names ).load( _ccf );
}

void SceneGraph::loadGeom( std::string _names, HttpResouceCB _ccf ) {
    B<GRB>( _names ).load( _ccf );
}

void SceneGraph::setMaterialRemap( const MaterialMap& _materialRemap ) {
    materialRemap = _materialRemap;
}

std::string SceneGraph::possibleRemap( const std::string& _key, const std::string& _value ) const {
    if ( const auto& it = materialRemap.find( _key + "," + _value ); it != materialRemap.end()) {
        return it->second;
    }
    return _value;
}

void SceneGraph::HODResolve( const DependencyList& deps, HODResolverCallback ccf ) {
    SceneDependencyResolver sdr( *this );

    sdr.addDeps( deps );
    sdr.addResolveCallback( ccf );

    dependencyResovlers.push_back( sdr );
    dependencyResovlers.back().resolve();
}

void HOD::DepRemapsManager::addDep( const std::string& group, const std::string& resName ) {
    ret.emplace( group, resName );
    if ( group == ResourceGroup::Geom ) {
        geoms.emplace( resName );
    }
}

void HOD::reducer( SceneGraph& sg, HOD::DepRemapsManager& deps, HODResolverCallback ccf ) {

    HOD::EntityList el{ deps.geoms };
    Http::get( Url{ HttpFilePrefix::entities + "remaps" }, el.serialize(), [&, deps, ccf]( HttpResponeParams res ) {
        EntityRemappingContainer erc{ res.bufferString };
        HOD::DepRemapsManager ndeps = deps;
        AppMaterialsRemapping remaps{};
        for ( const auto& rm : erc.remaps ) {
            remaps.remap[rm.sourceEntity + "," + rm.sourceRemap] = rm.destRemap;
            ndeps.addDep( ResourceGroup::Material, rm.destRemap );
        }

        sg.setMaterialRemap( remaps.remap );

        sg.HODResolve( ndeps.ret, ccf );
    } );
}
