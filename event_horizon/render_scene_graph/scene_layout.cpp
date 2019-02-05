//
// Created by Dado on 2018-10-29.
//

#include "scene_layout.h"
#include <core/app_globals.h>
#include <core/camera.h>
#include "graphics/camera_rig.hpp"
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/editor/includes.h>

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

SceneLayout::SceneLayout( InitLayoutFunction&& initLayout, RenderFunction&& _renderFunction, DragAndDropFunction&& _dd,
						  InitializeWindowFlagsT initFlags ) :
		initLayout( initLayout ), renderFunction(_renderFunction), dragAndDropFunc(_dd), initFlags( initFlags ) {
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Console, ImGuiConsoleLayout );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Geom, ImGuiGeoms );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Material, ImGuiMaterials );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Image, ImGuiImages );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Camera, ImGuiCamera );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Login, ImGuiLogin );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Timeline, ImGuiTimeline );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudMaterial, ImGuiCloudEntitiesMaterials );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudGeom, ImGuiCloudEntitiesGeom );
}

void SceneLayout::setDragAndDropFunction( DragAndDropFunction dd ) {
	dragAndDropFunc = dd;
}

void SceneLayout::addBox( const std::string& _name, float _l, float _r, float _t, float _b,
						  RenderLayoutFunction&& rlf ) {
	boxes[_name] = { { _l, _r, _t, _b}, CameraControls::Edit2d, rlf };
}

void SceneLayout::addBox( const std::string& _name, float _l, float _r, float _t, float _b, CameraControls _cc ) {
	boxes[_name] = { { sPresenterArrangerLeftFunction3d,
					   sPresenterArrangerRightFunction3d,
					   sPresenterArrangerTopFunction3d,
					   sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, _cc, nullptr };
}

void SceneLayout::addBox( const std::string& _name, float _l, float _r, float _t, float _b ) {
	if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
		boxes[_name] = { { _l, _r, _t, _b}, CameraControls::Edit2d, rlf->second };
	}
}

void SceneLayout::activate( Scene* _target ) {

	initLayout( this, _target );

	for ( auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Plan2d ) {
			_target->addViewport<RLTargetPlain>( k, v.updateAndGetRect(), v.cc, BlitType::OnScreen );
		} else if ( v.cc == CameraControls::Walk || v.cc == CameraControls::Fly ) {
			_target->addViewport<RLTargetPBR>( k, v.updateAndGetRect(), v.cc, BlitType::OnScreen );
		}
	}
}

void SceneLayout::resizeCallback( Scene* _target, const Vector2i& _resize ) {
	for ( auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Fly ) {
			auto r = v.updateAndGetRect();
			_target->getTarget( k )->resize( r );
			_target->CM().getRig(k)->setViewport( r );
		}
	}
}

void initDefaultLayout( SceneLayout* _layout, Scene* _target ) {
    _layout->addBox(Scene::DC(), 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );
}

std::shared_ptr<SceneLayout> SceneLayout::makeDefault() {
    return std::make_shared<SceneLayout>(initDefaultLayout);
}
