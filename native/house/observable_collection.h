#pragma once

#include <functional>
#include <memory>
#include <vector>

class ObservableCollection;
class HierGeom;

class ChangeInfo {
public:
	ChangeInfo() {}
	ChangeInfo( const std::vector<GeomSP>& newObjects, const std::vector<int64_t>& removedObjects ) : addedObjects( newObjects ), removedHashes( removedObjects ) {}
	std::shared_ptr<ObservableCollection> collection;
	std::vector<GeomSP> addedObjects;
	std::vector<int64_t> removedHashes;
	std::vector<GeomSP> modifiedObjects;
};

class ObservableCollection : public std::enable_shared_from_this<ObservableCollection> {
public:
	void registerCallback( const std::function<void( std::shared_ptr<ChangeInfo> )>& callback ) {
		callbacks.push_back( callback );
	}

	void notifyChanges( std::shared_ptr<ChangeInfo> e ) {
		if ( callbacks.size() == 0 ) return;

		e->collection = shared_from_this();
		for ( auto c : callbacks ) c( e );
	}

private:
	std::vector<std::function<void( std::shared_ptr<ChangeInfo> )>> callbacks;
};
