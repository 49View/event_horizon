#include "vr_element_manager.h"

#include "ray_tracing/scene_manager.h"
#include "vr_manager.hpp"

#include "vr/floor_navigator.h"
#include "vr/vr_floorplan_map.h"
//#include "cpp_common/vr/vr_furniture_selector.h"
#include "vr/vr_pointer.h"

void VRElementManager::init() {
	mVRPointer = std::make_shared<VRPointer>();
	mFloorNavigator = std::make_shared<FloorNavigator>();
	//mVRFloorplanMap = std::make_shared<VRFloorplanMap>();
	//mFurnitureSelector = std::make_shared<VRFurnitureSelector>();
}

void VRElementManager::update() {
	if ( !VRM.IsOn() ) return;

	mVRPointer->update();
	//mVRFloorplanMap->update();

	//mFurnitureSelector->update();
}

void VRElementManager::load() {
	mVRPointer->reset();

	mFloorNavigator->reset();
	//mVRFloorplanMap->reset();

	mVRPointer->registerTriggerableItem( mFloorNavigator );
	//mVRPointer->registerTriggerableItem( mFurnitureSelector );

	//mVRPointer->registerTriggerableItem(mVRFloorplanMap);
}