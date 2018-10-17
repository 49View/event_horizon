#pragma once

#include <memory>
#include <vector>

#include "house/observable_collection.h"

class HierGeom;

class StructuralCollection : public ObservableCollection {
public:
	StructuralCollection();
	std::vector<std::shared_ptr<HierGeom>> getAllObjects() const;

	void push_back( std::shared_ptr<HierGeom> _g );
	void clear();

private:
	bool includeGeom( std::shared_ptr<HierGeom> g );
	std::vector<std::shared_ptr<HierGeom>> geoms;
	std::vector<std::string> excludeGeoms;
};
