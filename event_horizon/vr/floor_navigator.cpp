#include "floor_navigator.h"
#include "graphics/render_list.h"
#include "vr/vr_ui_context.h"
#include "vr_manager.hpp"

void FloorNavigator::executeTriggerPressed( const TriggerPressedInfo& i ) {
	//	LOGR("Trigger was pressed at %s", i.hitInfo.finalPosition.toString().c_str());
	auto lGoingToPos = VRM.hmdPos().getPosition3() + i.hitInfo.finalPosition * Vector3f( -1.0f, -1.0f, 1.0f );
	//	LOGR( "going to %s", tmp.toString().c_str() );
	CM.getMainCameraRig( CurrentVrContext.CameraRigaName() )->goTo( lGoingToPos, 0.0f );
}

bool FloorNavigator::isManagingObject( int64_t id ) {
	return walkableHashes.find( id ) != walkableHashes.end();
}

void FloorNavigator::reset() {
	for ( auto g : CurrentVrContext.WalkableGeoms() ) {
		walkableHashes.insert( g->Hash() );
	}
}