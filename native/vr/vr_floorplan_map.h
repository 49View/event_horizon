#pragma once
#include <memory>
#include <vector>
#include "vr/triggerable.h"
#include "vr/vr_ui_element.h"

//class HouseMenu;

class VRFloorplanMap : public Triggerable, public VRUIElement {
public:
	VRFloorplanMap();
	void update();
	void reset();
	bool isManagingObject( int64_t id );
	void executeTriggerPressed( const TriggerPressedInfo& i );

private:
	//	bool mIsVisible = true;
	//	bool useRightHand = false;
	//	std::unique_ptr<HouseMenu> menu;
};
