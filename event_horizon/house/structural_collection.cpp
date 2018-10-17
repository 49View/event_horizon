#include "structural_collection.h"

#include <stack>
#include "graphics/render_list.h"
#include "poly/hier_geom.hpp"

StructuralCollection::StructuralCollection() {
	excludeGeoms.push_back( "Mesh8" ); //hinges
	excludeGeoms.push_back( "Maxlight_ES_200811_plafon_FORM:Sphere002" ); //spotlights
	excludeGeoms.push_back( "Maxlight_ES_200811_plafon_FORM:Cylinder004" ); //spotlights
	excludeGeoms.push_back( "Maxlight_ES_200811_plafon_FORM:Sphere001" ); //spotlights
}

bool StructuralCollection::includeGeom(std::shared_ptr<HierGeom> g) {
	if (!g->hasDataBacked()) return false;

	for ( auto s : excludeGeoms ) {
		if ( g->Name() == s )
			return false;
	}

	/*float opactity;
	g->BakedGeom()->getMaterial()->Uniforms()->get(UniformNames::opacity, opactity);
	if (opactity < 0.2f)  return false;*/

	return true;
}

std::vector<std::shared_ptr<HierGeom>> StructuralCollection::getAllObjects() const {
	return geoms;
}

void StructuralCollection::push_back( std::shared_ptr<HierGeom> _g ) {

	geoms.push_back( _g );
	auto ev = std::make_shared<ChangeInfo>();
	ev->addedObjects = { _g };

	notifyChanges( ev );
}

void StructuralCollection::clear() {
	std::vector<int64_t> old( geoms.size() );
	std::transform( geoms.begin(), geoms.end(), old.begin(), []( auto g ) {return g->Hash(); } );

	geoms.clear();

	auto ev = std::make_shared<ChangeInfo>();
	ev->removedHashes = old;
	notifyChanges( ev );
}
