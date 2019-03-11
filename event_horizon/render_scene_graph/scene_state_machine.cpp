//
// Created by Dado on 2018-10-29.
//

#include "scene_state_machine.h"
#include <core/app_globals.h>
#include <core/camera.h>
#include <core/camera_rig.hpp>
#include <graphics/render_targets.hpp>
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

SceneStateMachineBackEnd::SceneStateMachineBackEnd( SceneOrchestrator* _p ) : SceneOrchestratorDependency(_p) {

    stateMachine = std::make_unique<msm::back::state_machine<SceneStateMachine>>(this);

	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Taskbar,
			std::make_shared<ImGuiTaskbar>(SceneLayoutDefaultNames::Taskbar) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Console,
			std::make_shared<ImGuiConsoleLayout>(SceneLayoutDefaultNames::Console) );
	boxFunctionMapping.emplace( SceneLayoutDefaultNames::Scene,
			std::make_shared<ImGuiGeoms>(SceneLayoutDefaultNames::Scene) );
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

void SceneStateMachineBackEnd::addBoxToViewport( const std::string& _name, const Boxes& _box ) {
    if ( boxes.find(_name) != boxes.end() ) return;

	boxes[_name] = _box;

	if ( _box.cc != CameraControls::Edit2d ) {
		auto lViewport = boxes[_name].updateAndGetRect();

		auto rig = CameraBuilder{_name}.makeDefault(lViewport, o()->RSG());
        rig->setViewport(lViewport);

		RenderTargetType rtt = RenderTargetType::PBR;
		if ( boxes[_name].cc == CameraControls::Plan2d ) {
			rtt = RenderTargetType::Plain;
		}
		o()->addViewport( rtt, rig, lViewport, boxes[_name].cc, BlitType::OnScreen );
	}
}

void SceneStateMachineBackEnd::addBox( const std::string& _name, float _l, float _r, float _t, float _b, CameraControls _cc ) {
	addBoxToViewport( _name,{ { sPresenterArrangerLeftFunction3d,
					   sPresenterArrangerRightFunction3d,
					   sPresenterArrangerTopFunction3d,
					   sPresenterArrangerBottomFunction3d, _l, _r, _b, _t }, _cc, nullptr } );
}

void SceneStateMachineBackEnd::addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible ) {
	if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
		addBoxToViewport( _name,{ { _l, _r, _t, _b}, CameraControls::Edit2d, rlf->second } );
		boxes[_name].setVisible( _bVisible );
	}
}

void SceneStateMachineBackEnd::resizeCallback( SceneOrchestrator* _target, const Vector2i& _resize ) {
	for ( auto& [k,v] : boxes ) {
		orBitWiseFlag( v.flags, BoxFlags::Resize );
		if ( v.cc == CameraControls::Fly ) {
			auto r = v.updateAndGetRect();
			_target->RSG().RR().getTarget( k )->resize( r );
			_target->RSG().CM().get(k)->setViewport( r );
		}
	}
}

void SceneStateMachineBackEnd::activate() {
    stateMachine->process_event( SceneStateMachine::OnActivate() );
}

//void initDefaultLayout( SceneStateMachineBackEnd* _layout, SceneOrchestrator* _target ) {
//    _layout->addBox(SceneOrchestrator::DC(), 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );
//}
//
//std::shared_ptr<SceneStateMachineBackEnd> SceneStateMachineBackEnd::makeDefault() {
//    return std::make_shared<SceneStateMachineBackEnd>(initDefaultLayout);
//}

Rect2f& SceneStateMachineBackEnd::Boxes::updateAndGetRect() {
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

Rect2f SceneStateMachineBackEnd::Boxes::getRect() const {
    return rectArranger.getRect();
}

void SceneStateMachineBackEnd::Boxes::render( SceneOrchestrator *_target, Rect2f& _rect ) {
    if ( renderer ) {
        renderer->render( _target, _rect, flags );
    }
}

void SceneStateMachineBackEnd::Boxes::toggleVisible() {
    toggle(flags, BoxFlags::Visible);
    if ( renderer ) renderer->toggleVisible();
}

void SceneStateMachineBackEnd::Boxes::setVisible( bool _bVis ) {
    orBitWiseFlag( flags, BoxFlags::Visible );
    if ( renderer ) renderer->setVisible(_bVis);
}

void SceneStateMachineBackEnd::postDefaults() {
    // These are the defaults what will be set in case they haven't been set already in init() virtual

    // Set a fullscreen camera in case there's none
    addBox( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f, CameraControls::Fly );
}

void SceneStateMachineBackEnd::initNV() {
	o()->defaults();

	init();

	postDefaults();
}

void SceneStateMachine::activate( [[maybe_unused]] const SceneStateMachine::OnActivate& ) {
    owner->initNV();
}
