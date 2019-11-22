#include "vr_pointer.h"
#include "graphics/render_list.h"
#include "core/service_factory.h"
#include "ray_tracing/scene_manager.h"

#include "vr/triggerable.h"

#include "vr_manager.hpp"

VRPointer::VRPointer() {
	sceneManager = ServiceFactory::get<SceneManager>();
}

void VRPointer::registerTriggerableItem( std::shared_ptr<Triggerable> triggerableItem ) {
	triggerableItems.push_back( triggerableItem );
}

void VRPointer::reset() {
}

void VRPointer::update() {
	if ( !VRM.IsOn() ) return;

	auto controllerInfo = useRightHand ? VRM.RightHandController() : VRM.LeftHandController();

	if ( !controllerInfo.isValid ) return;

	lastHitInfo = sceneManager->testRay( controllerInfo.position, controllerInfo.direction );

	if ( !controllerInfo.isTriggerPressed && wasTriggerPressed ) {
		TriggerPressedInfo triggerPressedInfo;
		triggerPressedInfo.hitInfo = lastHitInfo;

		for ( auto item : triggerableItems ) {
			if ( item->isManagingObject( lastHitInfo.objectId ) ) {
				item->executeTriggerPressed( triggerPressedInfo );
			}
		}
	}

	//draw the pointer
	if ( lastHitInfo.isHit ) {
		RL.VPListPost3d()->setMaterialConstantOn( "pointerUp", UniformNames::alpha, 1.0f );
		RL.VPListPost3d()->setMaterialConstantOn( "pointerDown", UniformNames::alpha, 1.0f );
		drawCircle( RL.VPListPost3d(), lastHitInfo.finalPosition, lastHitInfo.normal, 0.02f * sqrt( log( lastHitInfo.distance + 1 ) ), Color4f::BLUE, 1000, "pointerUp" );
		drawCircle( RL.VPListPost3d(), lastHitInfo.finalPosition, lastHitInfo.normal * -1, 0.02f * sqrt( log( lastHitInfo.distance + 1 ) ), Color4f::BLUE, 1000, "pointerDown" );
	} else {
		RL.VPListPost3d()->setMaterialConstantOn( "pointerUp", UniformNames::alpha, 0.0f );
		RL.VPListPost3d()->setMaterialConstantOn( "pointerDown", UniformNames::alpha, 0.0f );
	}

	wasTriggerPressed = controllerInfo.isTriggerPressed;
}