//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"
#include <poly/scene_events.h>
#include <core/math/plane3f.h>
#include <core/raw_image.h>
#include <core/TTF.h>
#include <core/camera.h>
#include <core/resources/profile.hpp>
#include <core/resources/material.h>
#include <render_scene_graph/render_orchestrator.h>
#include <core/resources/resource_builder.hpp>
#include <core/math/vector_util.hpp>
#include <core/lightmap_exchange_format.h>
#include <graphics/lightmap_manager.hpp>
#include <graphics/light_manager.h>

scene_t scene{ 0 };

//const std::string testLoad = "rock";

void EditorBackEnd::activatePostLoad() {

    allCallbacksEntitySetup();
    rsg.setDragAndDropFunction( allConversionsDragAndDropCallback );

    backEnd->process_event( OnActivate{} );

    rsg.createSkybox( SkyBoxInitParams{ SkyBoxMode::CubeProcedural } );

//    rsg.createSkybox( SkyBoxInitParams{ SkyBoxMode::EquirectangularTexture,
//                                        sg.getHash<RawImage>( "skybox,equirectangular,park,generic,001" ) } );

    rsg.useSkybox( false );
    rsg.RR().LM()->setShadowZFightCofficient(0.02f);
    rsg.changeTime( "winter noon" );
    rsg.setRigCameraController<CameraControlOrbit3d>();
    rsg.DC()->setFoV(60.0f);

//        sg.load<Geom>( "window", [this](HttpResouceCBSign key) {
//        sg.GB<GT::Asset>( key, V3f::X_AXIS*3.0f );
//    } );

//    sg.GB<GT::Shape>( ShapeType::Sphere, GT::M( testLoad ));
}

void EditorBackEnd::activateImpl() {

//    appData.addRawImage( "skybox,equirectangular,park,generic,001" );
//    appData.addMaterial( testLoad );

    loadSceneEntities();

//    Http::get( Url{ "/user/portaltoload" },
//       [&]( const Http::Result& _res ) {
//           PortalToLoad entity{_res.bufferString};
//           if ( entity.group == ResourceGroup::Geom ) {
//               sg.load<Geom>( entity.entity_id, [this]( HttpResouceCBSign key ) {
//                   auto geom = sg.GB<GT::Asset>( key );
//                   rsg.DC()->center( geom->BBox3dCopy() );
//               } );
//           } else if ( entity.group == ResourceGroup::Material ) {
//               sg.load<Material>( entity.entity_id, [this, entity]( HttpResouceCBSign key ) {
//                   auto geom = sg.GB<GT::Shape>( ShapeType::Sphere, GT::Tag(1001) );
//                   rsg.RR().changeMaterialOnTags( { 1001, entity.hash } );
//                   rsg.DC()->center( geom->BBox3dCopy(), CameraCenterAngle::Back );
//               } );
//           }
//    } );

//                   auto geom = sg.GB<GT::Shape>( ShapeType::Sphere, GT::Tag(1001) );

//    rsg.RR().createGridV2( CommandBufferLimits::UnsortedStart, 1.0f, Color4f::DARK_GRAY,
//                           (Color4f::PASTEL_GRAYLIGHT*1.35f).A(1.0f), V2f{ 5.0f }, 0.02f );

//    rsg.skyBoxDeltaInterpolation()->value = 0.0f;
//    sg.GB<GT::Shape>( ShapeType::Cube, V3f::UP_AXIS*0.32, GT::Scale( 0.6f ) );

//    sg.load<Geom>( "bed", [this](HttpResouceCBSign key) {
//        sg.GB<GT::Asset>( key, V3f::X_AXIS*3.0f );
//    } );
//    sg.load<Geom>( "lauter,selije,bedside", [this](HttpResouceCBSign key) {
//        sg.GB<GT::Asset>( key );
//    } );

//    sg.load<Geom>( "vitra", [this](HttpResouceCBSign key) {
//        sg.GB<GT::Asset>( key );
//    } );

//    sg.load<Geom>("curtain", [this](HttpResouceCBSign key) {
//        sg.addNode( key );
//    } );
//    sg.dumpAsObjFile();
//    rsg.RR().drawRect2d( CommandBufferLimits::UI2dStart, V2f{0.03f, 0.96f}, V2f{ 0.03f * 0.02f, 0.98f}, C4f::GREEN  );
}

void EditorBackEnd::updateImpl( const AggregatedInputData& _aid ) {

    if ( _aid.ti.checkKeyToggleOn( GMK_Z )) {
        sg.chartMeshes2( scene );
        LightmapManager::initScene( &scene, rsg.RR());
        LightmapManager::bake( &scene, rsg.RR());
        LightmapManager::apply( scene, rsg.RR());
    }
}
