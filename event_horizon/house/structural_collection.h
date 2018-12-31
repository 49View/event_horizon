#pragma once

#include <memory>
#include <vector>

#include "house/observable_collection.h"

class HierGeom;

class StructuralCollection : public ObservableCollection {
public:
	StructuralCollection();
	std::vector<GeomAssetSP> getAllObjects() const;

	void push_back( GeomAssetSP _g );
	void clear();

private:
	bool includeGeom( GeomAssetSP g );
	std::vector<GeomAssetSP> geoms;
	std::vector<std::string> excludeGeoms;
};
