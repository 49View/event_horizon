#pragma once

#include "poly/hier_geom.hpp"

class VRUIContext {
public:
	static VRUIContext& getInstance() {
		static VRUIContext instance;
		return instance;
	}

	std::string CameraRigaName() const { return cameraRigaName; }
	void CameraRigaName( std::string val ) { cameraRigaName = val; }

	std::vector<std::shared_ptr<HierGeom>> WalkableGeoms() const { return walkableGeoms; }
	void WalkableGeoms( std::vector<std::shared_ptr<HierGeom>> val ) { walkableGeoms = val; }
private:
	VRUIContext() {
		cameraRigaName = "Main";
	}

	VRUIContext( VRUIContext const& ) = delete;
	void operator=( VRUIContext const& ) = delete;
	~VRUIContext() {}

private:
	std::string cameraRigaName;
	std::vector<std::shared_ptr<HierGeom>> walkableGeoms;
};

#define CurrentVrContext VRUIContext::getInstance()