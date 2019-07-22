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

JSONDATA( PortalToLoad, group, hash, entity_id )
    std::string group;
    std::string hash;
    std::string entity_id;
};

void EditorBackEnd::activateImpl() {

    allCallbacksEntitySetup();
    rsg.setDragAndDropFunction( allConversionsDragAndDropCallback );

    backEnd->process_event( OnActivate{} );

    rsg.createSkybox( SkyBoxInitParams{ SkyBoxMode::CubeProcedural } );
    rsg.useSkybox( false );
    rsg.RR().LM()->setShadowZFightCofficient(0.02f);
    rsg.changeTime( "winter noon" );
    rsg.setRigCameraController<CameraControlOrbit3d>();
    rsg.DC()->setFoV(60.0f);

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

    Socket::emit("{ \"msg\": \"requestAsset\"}");
//    rsg.RR().createGridV2( CommandBufferLimits::UnsortedStart, 1.0f, Color4f::DARK_GRAY,
//                           (Color4f::PASTEL_GRAYLIGHT*1.35f).A(1.0f), V2f{ 5.0f }, 0.02f );

//    rsg.skyBoxDeltaInterpolation()->value = 0.0f;
//    sg.GB<GT::Shape>( ShapeType::Cube, V3f::UP_AXIS*0.32, GT::Scale( 0.6f ) );
//    sg.GB<GT::Shape>( ShapeType::Cube, GT::Scale( 5.f, 0.01f, 5.f ) );

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


//#include <core/resources/resource_utils.hpp>
//#include <core/camera_rig.hpp>
//#include <core/camera.h>
//#include <render_scene_graph/scene_bridge.h>
//#include <poly/resources/ui_shape_builder.h>
//#include <poly/resources/geom_builder.h>
//#include <media/audio_video_stream.hpp>
//#include "callbacks_layout.h"
//
//void FullEditorStateMachine::init() {
//
//    struct UIViewLayout {
//        float consoleHeight = 0.0f;
//        float rightPanelWidth = 0.0f;
//        float leftPanelHeight = 0.0f;
//        float leftPanelHeight2 = 0.0f;
//        float rightPanelHeight = 0.0f;
//        float loginPanelHeight = 0.0f;
//        float taskbarHeight = 0.05f;
//        Vector2f main3dWindowSize = Vector2f::ZERO;
//        Vector2f timeLinePanelSize = Vector2f::ZERO;
//        Rect2f foxLayout = Rect2f::ZERO;
//    };
//
//    UIViewLayout uivl;
//
//    uivl.consoleHeight = 0.15f;
//    uivl.rightPanelWidth = 0.25f;
//    uivl.loginPanelHeight = 0.18f;
//    uivl.rightPanelHeight = ((1.0f - uivl.consoleHeight)/2.0f) - uivl.loginPanelHeight*0.5f;
//    uivl.leftPanelHeight = (1.0f - uivl.consoleHeight)/3.0f;
//    uivl.leftPanelHeight2 = (1.0f - uivl.consoleHeight)/5.f;
//    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.35f };
//    float timeLineY = 1.0f-(uivl.consoleHeight+uivl.timeLinePanelSize.y());
//
//#define CENTER(xc,yc) 0.5f-xc*0.5f, 0.5f+xc*0.5f, 0.5f-yc*0.5f, 0.5f+yc*0.5f
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Taskbar, 0.0f, 1.0f, 0.0f, uivl.taskbarHeight );
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Login, CENTER(uivl.rightPanelWidth, uivl.loginPanelHeight), false);
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Console, 0.0f, 1.0f, 1.0f-uivl.consoleHeight, 1.0f );
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Scene, 0.0f, uivl.rightPanelWidth, uivl.taskbarHeight, timeLineY + uivl.taskbarHeight );
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Material, 0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight, uivl.leftPanelHeight*2.0f - uivl.taskbarHeight, false );
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Image, CENTER(uivl.rightPanelWidth, uivl.leftPanelHeight2*5.0f), false );
////    so->StateMachine()->addBox( SceneLayoutDefaultNames::Camera, 1.0f-uivl.rightPanelWidth, 1.0f,
////                     timeLineY - uivl.leftPanelHeight2 + uivl.taskbarHeight,
////                     timeLineY+ uivl.taskbarHeight);
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::Timeline,
//                     0.0f, 1.0f,
//                     timeLineY +uivl.taskbarHeight, timeLineY + uivl.timeLinePanelSize.y() );
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::CloudMaterial,
//                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight, false );
//
//    so->StateMachine()->addBox( SceneLayoutDefaultNames::CloudGeom,
//                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight + uivl.rightPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight*2, false );
//
//    float topX = uivl.rightPanelWidth;
//    float cameraWidth = (1.0f-uivl.rightPanelWidth*2.0f);
//    float cameraAspectRatio = (720.0f / 1280.0f);
//    float cameraHeight = cameraWidth*(cameraAspectRatio*(1280.0f/720.0f));
//    so->StateMachine()->addRig<CameraControl2d>( Name::Foxtrot,
//                             topX, topX + cameraWidth,
//                             uivl.taskbarHeight, cameraHeight + uivl.taskbarHeight );
//
//    allCallbacksEntitySetup();
//    so->setDragAndDropFunction(allConversionsDragAndDropCallback);
//
////        auto streamName = "http://192.168.1.123:8080/video";
////        o()->addHttpStream<AudioVideoStreamFFmpeg>(streamName);
////        GB{ShapeType::Panel}.m(S::YUV_GREENSCREEN,streamName).build(o()->RSG());
//
//}
