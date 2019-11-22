#pragma once

#include <memory>
#include <vector>

#include "house/observable_collection.h"

class HierGeom;

class StructuralCollection : public ObservableCollection {
public:
	StructuralCollection();
	std::vector<GeomSP> getAllObjects() const;

	void push_back( GeomSP _g );
	void clear();

private:
	bool includeGeom( GeomSP g );
	std::vector<GeomSP> geoms;
	std::vector<std::string> excludeGeoms;
};
