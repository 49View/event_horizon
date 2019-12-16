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
#include <graphics/render_light_manager.h>
#include <graphics/shader_manager.h>

scene_t scene{ 0 };

//const std::string testLoad = "rock";
//const std::string skyboxName = "barcelona";
const std::string skyboxName = "tropical,beach";

void EditorBackEnd::activatePostLoad() {

//    allCallbacksEntitySetup();
    rsg.setDragAndDropFunction( allConversionsDragAndDropCallback );

    backEnd->process_event( OnActivate{} );

    rsg.createSkybox( SkyBoxInitParams{ SkyBoxMode::CubeProcedural } );

//    rsg.createSkybox( SkyBoxInitParams{ SkyBoxMode::EquirectangularTexture,
//                                        sg.getHash<RawImage>( skyboxName ) } );

    Renderer::clearColor(C4f::WHITE);
    rsg.useSkybox( true );
    rsg.RR().LM()->setShadowZFightCofficient(0.02f);
    rsg.RR().LM()->setIndoorSceneCoeff(2.0f);
    rsg.changeTime( "winter noon" );
    rsg.setRigCameraController<CameraControlOrbit3d>();
    rsg.DC()->setFoV(60.0f);

//    sg.addGeomScene("5d60844ea683f7913c3a3f6e");
//        sg.load<Geom>( "window", [this](HttpResouceCBSign key) {
//        sg.GB<GT::Asset>( key, V3f::X_AXIS*3.0f );
//    } );

//    sg.loadMaterial("carpet");
//    sg.GB<GT::Shape>( ShapeType::Sphere, GT::M( "carpet" ));
}

void EditorBackEnd::activateImpl() {

//    appData.addRawImage( "49viewlogo" );
//    appData.addRawImage( skyboxName );
//    appData.addFont("amaranth");
//    appData.addMaterial( "carpet" );

    loadSceneEntities();

//    sg.GB<GT::Shape>( ShapeType::Cylinder, GT::Tag(1001),  GT::M( "carpet" ) );

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

    if ( _aid.TI().checkKeyToggleOn( GMK_Z )) {
        sg.chartMeshes2( scene );
        LightmapManager::initScene( &scene, rsg.RR());
        LightmapManager::bake( &scene, rsg.RR());
        LightmapManager::apply( scene, rsg.RR());
    }
}