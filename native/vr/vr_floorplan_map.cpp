#include "vr_floorplan_map.h"

#include "vr/vr_ui_context.h"
#include "graphics/render_list.h"
#include "house/house_menu.h"

#include "vr_manager.hpp"

VRFloorplanMap::VRFloorplanMap() {
}

void VRFloorplanMap::update() {
	auto controllerInfo = VRM.LeftHandController();

	if ( !controllerInfo.isValid ) return;

	if ( controllerInfo.isTriggerPressed ) {
	}

	//draw the pointer
	//if ( lastHitInfo.isHit ) {
	//	RL.VPListPost3d()->remove( "pointerUp" );
	//	RL.VPListPost3d()->remove( "pointerDown" );

	//	RL.VPListPost3d()->drawCircle( lastHitInfo.finalPosition, lastHitInfo.normal, 0.02f * sqrt( log( lastHitInfo.distance + 1 ) ), C4fc::BLUE, 1000, "pointerUp" );
	//	RL.VPListPost3d()->drawCircle( lastHitInfo.finalPosition, lastHitInfo.normal * -1, 0.02f * sqrt( log( lastHitInfo.distance + 1 ) ), C4fc::BLUE, 1000, "pointerDown" );
	//}
}

void VRFloorplanMap::reset() {
	//	menu = std::make_unique<HouseMenu>();
	//	menu->SetLayout(1, 1, false, false);
	//	menu->init();
}

bool VRFloorplanMap::isManagingObject( int64_t /*id*/ ) {
	throw std::logic_error( "The method or operation is not implemented." );
}

void VRFloorplanMap::executeTriggerPressed( const TriggerPressedInfo& /*i*/ ) {
	throw std::logic_error( "The method or operation is not implemented." );
}