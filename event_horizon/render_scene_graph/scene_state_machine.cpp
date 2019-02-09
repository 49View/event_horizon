//
// Created by Dado on 2018-10-29.
//

#include "scene_state_machine.h"
#include <core/app_globals.h>
#include <core/camera.h>
#include "graphics/camera_rig.hpp"
#include <render_scene_graph/scene_orchestrator.hpp>
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

SceneStateMachine::SceneStateMachine( SceneOrchestrator* _p ) : orchestrator(_p) {
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Taskbar,
			std::make_shared<ImGuiTaskbar>(SceneLayoutDefaultNames::Taskbar) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Console,
			std::make_shared<ImGuiConsoleLayout>(SceneLayoutDefaultNames::Console) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Geom,
			std::make_shared<ImGuiGeoms>(SceneLayoutDefaultNames::Geom) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Material,
			std::make_shared<ImGuiMaterials>(SceneLayoutDefaultNames::Material) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Image,
			std::make_shared<ImGuiImages>(SceneLayoutDefaultNames::Image) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Camera,
			std::make_shared<ImGuiCamera>(SceneLayoutDefaultNames::Camera) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Login,
			std::make_shared<ImGuiLogin>(SceneLayoutDefaultNames::Login) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Timeline,
			std::make_shared<ImGuiTimeline>(SceneLayoutDefaultNames::Timeline) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudMaterial,
			std::make_shared<ImGuiCloudEntitiesMaterials>(SceneLayoutDefaultNames::CloudMaterial) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::CloudGeom,
			std::make_shared<ImGuiCloudEntitiesGeom>(SceneLayoutDefaultNames::CloudGeom) );
}

void SceneStateMachine::addBox( const std::string& _name, float _l, float _r, float _t, float _b,
						  std::shared_ptr<LayoutBoxRenderer> _lbr ) {
	boxes[_name] = { { _l, _r, _t, _b}, CameraControls::Edit2d, _lbr };
}

void SceneStateMachine::addBox( const std::string& _name, float _l, float _r, float _t, float _b, CameraControls _cc ) {
	boxes[_name] = { { sPresenterArrangerLeftFunction3d,
					   sPresenterArrangerRightFunction3d,
					   sPresenterArrangerTopFunction3d,
					   sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, _cc, nullptr };
}

void SceneStateMachine::addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible ) {
	if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
		boxes[_name] = { { _l, _r, _t, _b}, CameraControls::Edit2d, rlf->second };
		boxes[_name].setVisible( _bVisible );
	}
}

void SceneStateMachine::activate( SceneOrchestrator* _target ) {

	activateImpl();

	for ( auto& [k,v] : boxes ) {
		if ( v.cc == CameraControls::Plan2d ) {
			_target->addViewport<RLTargetPlain>( k, v.updateAndGetRect(), v.cc, BlitType::OnScreen );
		} else if ( v.cc == CameraControls::Walk || v.cc == CameraControls::Fly ) {
			_target->addViewport<RLTargetPBR>( k, v.updateAndGetRect(), v.cc, BlitType::OnScreen );
		}
	}
}

void SceneStateMachine::resizeCallback( SceneOrchestrator* _target, const Vector2i& _resize ) {
	for ( auto& [k,v] : boxes ) {
		orBitWiseFlag( v.flags, BoxFlags::Resize );
		if ( v.cc == CameraControls::Fly ) {
			auto r = v.updateAndGetRect();
			_target->getTarget( k )->resize( r );
			_target->CM().getRig(k)->setViewport( r );
		}
	}
}

//void initDefaultLayout( SceneStateMachine* _layout, SceneOrchestrator* _target ) {
//    _layout->addBox(SceneOrchestrator::DC(), 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );
//}
//
//std::shared_ptr<SceneStateMachine> SceneStateMachine::makeDefault() {
//    return std::make_shared<SceneStateMachine>(initDefaultLayout);
//}

Rect2f& SceneStateMachine::Boxes::updateAndGetRect() {
    if ( checkBitWiseFlag( flags, BoxFlags::Rearrange ) ) {
        rectArranger.set();
        xandBitWiseFlag( flags, BoxFlags::Rearrange );
    }
    if ( checkBitWiseFlag( flags, BoxFlags::Resize ) ) {
        rectArranger.resize();
        xandBitWiseFlag( flags, BoxFlags::Resize );
    }
//    rectArranger.updateScreenPerc();
    return rectArranger.getRect();
}

Rect2f SceneStateMachine::Boxes::getRect() const {
    return rectArranger.getRect();
}

void SceneStateMachine::Boxes::render( SceneOrchestrator *_target, Rect2f& _rect ) {
    if ( renderer ) {
        renderer->render( _target, _rect, flags );
    }
}

void SceneStateMachine::Boxes::toggleVisible() {
    toggle(flags, BoxFlags::Visible);
    if ( renderer ) renderer->toggleVisible();
}

void SceneStateMachine::Boxes::setVisible( bool _bVis ) {
    orBitWiseFlag( flags, BoxFlags::Visible );
    if ( renderer ) renderer->setVisible(_bVis);
}
