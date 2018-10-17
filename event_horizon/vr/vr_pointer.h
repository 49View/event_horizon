#pragma once
#include <memory>
#include <vector>

#include "vr/vr_ui_element.h"
#include "ray_tracing/hit_info.h"

class SceneManager;
class Triggerable;

class VRPointer : public VRUIElement {
public:
	VRPointer();
	void registerTriggerableItem( std::shared_ptr<Triggerable> triggerableItem );
	void reset();
	void update();

	HitInfo LastHitInfo() const { return lastHitInfo; }
private:
	std::vector<std::shared_ptr<Triggerable>> triggerableItems;
	bool wasTriggerPressed = false;
	bool useRightHand = true;
	HitInfo lastHitInfo;
	std::shared_ptr<SceneManager> sceneManager;
};