//
// Created by Dado on 2018-10-29.
//

#include "scene_bridge.h"
#include <core/app_globals.h>
#include <core/camera.h>
#include <core/camera_rig.hpp>
#include <core/app_globals.h>
#include <core/resources/resource_manager.hpp>
#include <graphics/render_targets.hpp>
#include <graphics/framebuffer.h>

float sPresenterArrangerLeftFunction( float _value ) {
    return getScreenSizefUI.x() * _value;
}

float sPresenterArrangerRightFunction( float _value ) {
    return getScreenSizefUI.x() * _value;
}

float sPresenterArrangerTopFunction( float _value ) {
    return getScreenSizefUI.y() * ( _value );
}

float sPresenterArrangerBottomFunction( float _value ) {
    return getScreenSizefUI.y() * _value;
}

float sPresenterArrangerLeftFunction3d( float _value ) {
    return getScreenSizef.x() * _value;
}

float sPresenterArrangerRightFunction3d( float _value ) {
    return getScreenSizef.x() * _value;
}

float sPresenterArrangerTopFunction3d( float _value ) {
    return getScreenSizef.y() - ( getScreenSizef.y() * ( _value ) );
}

float sPresenterArrangerBottomFunction3d( float _value ) {
    return getScreenSizef.y() - ( getScreenSizef.y() * ( _value ) );
}

//    ### REF move to editor side of code
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Taskbar,
//			std::make_shared<ImGuiTaskbar>(SceneLayoutDefaultNames::Taskbar) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Console,
//			std::make_shared<ImGuiConsoleLayout>(SceneLayoutDefaultNames::Console) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Scene,
//			std::make_shared<ImGuiGeoms>(SceneLayoutDefaultNames::Scene) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Material,
//			std::make_shared<ImGuiMaterials>(SceneLayoutDefaultNames::Material) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Image,
//			std::make_shared<ImGuiImages>(SceneLayoutDefaultNames::Image) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Camera,
//			std::make_shared<ImGuiCamera>(SceneLayoutDefaultNames::Camera) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Login,
//			std::make_shared<ImGuiLogin>(SceneLayoutDefaultNames::Login) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Timeline,
//			std::make_shared<ImGuiTimeline>(SceneLayoutDefaultNames::Timeline) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudMaterial,
//			std::make_shared<ImGuiCloudEntitiesMaterials>(SceneLayoutDefaultNames::CloudMaterial) );
//	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudGeom,
//			std::make_shared<ImGuiCloudEntitiesGeom>(SceneLayoutDefaultNames::CloudGeom) );

Rect2f& SceneScreenBox::updateAndGetRect() {
    if ( checkBitWiseFlag( flags, BoxFlags::Rearrange ) ) {
        rectArranger.set();
        xandBitWiseFlag( flags, BoxFlags::Rearrange );
    }
    if ( checkBitWiseFlag( flags, BoxFlags::Resize ) ) {
        rectArranger.resize();
        xandBitWiseFlag( flags, BoxFlags::Resize );
    }
    return rectArranger.getRect();
}

Rect2f SceneScreenBox::getRect() const {
    return rectArranger.getRect();
}

void SceneScreenBox::render( SceneOrchestrator *_target, Rect2f& _rect ) {
    if ( renderer ) {
        renderer->render( _target, _rect, flags );
    }
}

void SceneScreenBox::toggleVisible() {
    toggle(flags, BoxFlags::Visible);
    if ( renderer ) renderer->toggleVisible();
}

void SceneScreenBox::setVisible( bool _bVis ) {
    orBitWiseFlag( flags, BoxFlags::Visible );
    if ( renderer ) renderer->setVisible(_bVis);
}

void SceneRectArranger::resize() {
    rect.percentage( sizeScreenPerc, getScreenSizefUI );
//        updateScreenPerc();
}

void SceneRectArranger::updateScreenPerc() {
    sizeScreenPerc = Rect2f::percentage( rect, getScreenRectUI );
}
