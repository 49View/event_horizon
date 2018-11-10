//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"

#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/scene_layout.h>
#include "callbacks_layout.h"

struct UIViewLayout {
    float consoleHeight = 0.0f;
    float rightPanelWidth = 0.0f;
    float leftPanelHeight = 0.0f;
    float leftPanelHeight2 = 0.0f;
    float rightPanelHeight = 0.0f;
    Vector2f main3dWindowSize = Vector2f::ZERO;
    Vector2f timeLinePanelSize = Vector2f::ZERO;
    Rect2f foxLayout;
};

UIViewLayout uivl;

void initLayout( SceneLayout* _layout, [[maybe_unused]] Scene* p ) {

    uivl.consoleHeight = 0.15f;
    uivl.rightPanelWidth = 0.25f;
    uivl.rightPanelHeight = (1.0f - uivl.consoleHeight)/2.0f;
    uivl.leftPanelHeight = (1.0f - uivl.consoleHeight)/3.0f;
    uivl.leftPanelHeight2 = (1.0f - uivl.consoleHeight)/6.f;
    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.20f };
    float topX = uivl.rightPanelWidth;

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

    _layout->addBox( SceneLayoutDefaultNames::CloudMaterial,
                     1.0f-uivl.rightPanelWidth, 1.0f, 0.0f, uivl.rightPanelHeight );

    _layout->addBox( SceneLayoutDefaultNames::CloudGeom,
                     1.0f-uivl.rightPanelWidth, 1.0f, uivl.rightPanelHeight, uivl.rightPanelHeight*2 );

    _layout->addBox( Name::Foxtrot,
                     topX, topX + (1.0f-uivl.rightPanelWidth*2.0f),
                     0.0f, (1.0f-(uivl.consoleHeight + uivl.timeLinePanelSize.y())), CameraControls::Fly );

    allCallbacksEntitySetup();
}

void render( [[maybe_unused]] Scene* p ) {

}

std::shared_ptr<SceneLayout> fullEditor() {
    return std::make_shared<SceneLayout>(initLayout, render, allConversionsDragAndDropCallback);
}
