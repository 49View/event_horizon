//
//  sixthmaker
//
//  Created by Dado on 30/07/2017.
//
//

#pragma once

#include "osm_bsdata.hpp"
#include "core/service_factory.h"
#include "core/callback_dependency.h"
#include "core/observable.h"
#include "core/file_manager.h"
#include "render_scene_graph/di_modules.h"

struct OsmArea;

class OSMBuilder : public ResourceBuilderObservable, public Observable<OSMBuilder> {
public:
	virtual ~OSMBuilder() {}
	using ResourceBuilderObservable::ResourceBuilderObservable;
	template<typename T>
	bool build(std::shared_ptr<T> _observer) {
		subscribe(_observer);
		FM::readRemote<OSMBuilder, HttpQuery::JSON, OsmArea>( Name(), *this );
		return true;
	}
	bool make( uint8_p&& _data, const DependencyStatus _status ) override;
	const std::shared_ptr<OsmArea>& getOSMArea() const;
protected:
	std::shared_ptr<OsmArea> osmArea;
};

class OSMManager;

class CommandScriptOSMManager : public CommandScript {
public:
	CommandScriptOSMManager( OSMManager& hm );
	virtual ~CommandScriptOSMManager() {}
};

class OSMManager : public Observer<OSMBuilder>, public std::enable_shared_from_this<OSMManager> {
public:
	OSMManager( RenderSceneGraph& _sg, CommandQueue& cq ) : sg( _sg ) {
		hcs = std::make_shared<CommandScriptOSMManager>(*this);
		cq.registerCommandScript(hcs);
	}

	virtual ~OSMManager() {}
	virtual void notified( OSMBuilder& _source, const std::string& generator ) override;
	std::shared_ptr<OSMManager> getPtr() { return shared_from_this(); }

    void cmdLoadMap( const std::vector<std::string>& _params );

    std::vector<std::tuple<std::vector<Vector3f>, float, Color4f>> roadLines(const Vector3f& lcenter);
	std::tuple<std::vector<Vector3f>, std::vector<int32_t>> terrainGrid(const Vector3f& lcenter);
	std::vector<std::tuple<std::vector<Vector3f>, float>> buildings();

	float HeightScale() const { return heightScale; }
	void HeightScale( float val ) { heightScale = val; }
	Vector3f get3dLonLat(const Vector2f& longitudeLatitude, const Vector3f& lcenter);
	Vector3f get3dLatLon(const Vector2f& longitudeLatitude, const Vector3f& lcenter);
	Vector3f get3DCoord(const float longitude, const float latitude, const float elevation, const Vector3f& lcenter);
	Vector3f get3DCoord(const Vector2f& latitudeLongitude, const float elevation, const Vector3f& lcenter);
private:
	std::shared_ptr<OsmArea> bsdata;
	std::shared_ptr<CommandScriptOSMManager> hcs;
	RenderSceneGraph& sg;

	//std::map<uint64_t, const OSMNode*> vnodesMap;

	Rect2f bounds = Rect2f::INVALID;
	float heightScale = 0.0f;
	float medianLatitude = 0.0f;
	float latitudeScale = 0.0f;
};

class OSMProxy {
public:
	void createOSMProxy( RenderSceneGraph& _sg, CommandQueue& cq ) {
		osmProxy = std::make_shared<OSMManager>( _sg, cq );
	}
protected:
	std::shared_ptr<OSMManager> osmProxy;
};