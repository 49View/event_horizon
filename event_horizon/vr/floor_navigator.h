#pragma once
#include <unordered_set>
#include "vr/triggerable.h"

class FloorNavigator : public Triggerable {
public:
	void executeTriggerPressed( const TriggerPressedInfo& i );
	bool isManagingObject( int64_t id );
	void reset();
private:
	std::unordered_set<int64_t> walkableHashes;
};
