#pragma once
#include "ray_tracing/hit_info.h"

struct TriggerPressedInfo {
	HitInfo hitInfo;
};

class Triggerable {
public:
	virtual bool isManagingObject( int64_t id ) = 0;
	virtual void executeTriggerPressed( const TriggerPressedInfo& i ) = 0;
	virtual ~Triggerable() {};
};
