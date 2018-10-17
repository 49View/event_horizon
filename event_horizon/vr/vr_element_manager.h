#pragma once

#include <memory>

class VRPointer;
class FloorNavigator;
class VRFurnitureSelector;

class VRElementManager {
public:
	void init();
	void update();
	void load();
	std::shared_ptr<VRPointer> VrPointer() { return mVRPointer; }
private:
	std::shared_ptr<VRPointer> mVRPointer;
	std::shared_ptr<FloorNavigator> mFloorNavigator;
	//std::shared_ptr<VRFloorplanMap> mVRFloorplanMap;
	std::shared_ptr<VRFurnitureSelector> mFurnitureSelector;
};
