#pragma once

#include "core/node.hpp"

class VRUIContext {
public:
	static VRUIContext& getInstance() {
		static VRUIContext instance;
		return instance;
	}

	std::string CameraRigaName() const { return cameraRigaName; }
	void CameraRigaName( std::string val ) { cameraRigaName = val; }

	std::vector<GeomAssetSP> WalkableGeoms() const { return walkableGeoms; }
	void WalkableGeoms( std::vector<GeomAssetSP> val ) { walkableGeoms = val; }
private:
	VRUIContext() {
		cameraRigaName = "Main";
	}

	VRUIContext( VRUIContext const& ) = delete;
	void operator=( VRUIContext const& ) = delete;
	~VRUIContext() {}

private:
	std::string cameraRigaName;
	std::vector<GeomAssetSP> walkableGeoms;
};

#define CurrentVrContext VRUIContext::getInstance()