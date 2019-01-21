//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"

#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/scene_layout.h>
//#include <media/audio_video_stream.hpp>
//#include <poly/ui_shape_builder.h>
#include "callbacks_layout.h"

//std::unique_ptr<AudioVideoStream> avt;

void initLayout( SceneLayout* _layout, [[maybe_unused]] Scene* p ) {

    struct UIViewLayout {
        float consoleHeight = 0.0f;
        float rightPanelWidth = 0.0f;
        float leftPanelHeight = 0.0f;
        float leftPanelHeight2 = 0.0f;
        float rightPanelHeight = 0.0f;
        float loginPanelHeight = 0.0f;
        Vector2f main3dWindowSize = Vector2f::ZERO;
        Vector2f timeLinePanelSize = Vector2f::ZERO;
        Rect2f foxLayout = Rect2f::ZERO;
    };

    UIViewLayout uivl;

    uivl.consoleHeight = 0.15f;
    uivl.rightPanelWidth = 0.25f;
    uivl.loginPanelHeight = (1.0f - uivl.consoleHeight)/9.0f;
    uivl.rightPanelHeight = ((1.0f - uivl.consoleHeight)/2.0f) - uivl.loginPanelHeight*0.5f;
    uivl.leftPanelHeight = (1.0f - uivl.consoleHeight)/3.0f;
    uivl.leftPanelHeight2 = (1.0f - uivl.consoleHeight)/6.f;
    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.30f };
    float topX = uivl.rightPanelWidth;
    float cameraWidth = (1.0f-uivl.rightPanelWidth*2.0f);
    float cameraAspectRatio = (720.0f / 1280.0f);
    float cameraHeight = cameraWidth*(cameraAspectRatio*(1280.0f/720.0f));

    _layout->addBox( SceneLayoutDefaultNames::Console, 0.0f, 1.0f, 1.0f-uivl.consoleHeight, 1.0f );
    _layout->addBox( SceneLayoutDefaultNames::Geom, 0.0f, uivl.rightPanelWidth, 0.0f, uivl.leftPanelHeight );
    _layout->addBox( SceneLayoutDefaultNames::Material,
                     0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight, uivl.leftPanelHeight*2.0f );

    float imageTY = uivl.leftPanelHeight*2.0f + uivl.leftPanelHeight2;
    _layout->addBox( SceneLayoutDefaultNames::Image, 0.0f, uivl.rightPanelWidth, uivl.leftPanelHeight*2.0f, imageTY );

    _layout->addBox( SceneLayoutDefaultNames::Camera, 0.0f, uivl.rightPanelWidth, imageTY, imageTY + uivl.leftPanelHeight2 );

    float timeLineY = 1.0f-(uivl.consoleHeight+uivl.timeLinePanelSize.y());
    _layout->addBox( SceneLayoutDefaultNames::Timeline,
                     uivl.rightPanelWidth, uivl.rightPanelWidth + uivl.timeLinePanelSize.x(),
                     timeLineY, timeLineY + uivl.timeLinePanelSize.y() );

    _layout->addBox( SceneLayoutDefaultNames::Login,
                     1.0f-uivl.rightPanelWidth, 1.0f, 0.0f, uivl.loginPanelHeight );

    _layout->addBox( SceneLayoutDefaultNames::CloudMaterial,
                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight );

    _layout->addBox( SceneLayoutDefaultNames::CloudGeom,
                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.loginPanelHeight + uivl.rightPanelHeight, uivl.loginPanelHeight + uivl.rightPanelHeight*2 );

    _layout->addBox( Name::Foxtrot,
                     topX, topX + cameraWidth,
                     0.0f, cameraHeight, CameraControls::Fly );

    allCallbacksEntitySetup();

//    avt = std::make_unique<AudioVideoStream>(p->SSM());
//    avt->main_decode("http://192.168.1.123:8080/video");
//    UISB{ UIShapeType::Rect3d }.r(Rect2f{Vector2f::ZERO, Vector2f::ONE, true}).c(Color4f::WHITE).buildr(p->RSG());
//    UISB{ UIShapeType::Line3d }.v( {V3f::ZERO, V3f::X_AXIS}).lw(1.0f).c(Color4f::BLACK).buildr(p->RSG());

//    UISB{ UIShapeType::Arrow3d }.v( { XZY::C({ V2f::ZERO, 0.0f }),
//                                      XZY::C({ V2f::ONE, 0.0f }) } ).
//            lw( 0.1f ).c( Color4f::AQUAMARINE ).build(p->RSG());

//    UISB{ UIShapeType::Text3d, "Hello", 0.6f }.c(Color4f::AQUAMARINE).buildr(p->RSG());
}

void render( Scene* p ) {
//    if ( avt) avt->advanceFrame();
}

std::shared_ptr<SceneLayout> fullEditor() {
    return std::make_shared<SceneLayout>(initLayout, render, allConversionsDragAndDropCallback);
}
