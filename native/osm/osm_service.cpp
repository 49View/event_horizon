//
//
//  Created by Dado on 30/07/2017.
//
//

#include <poly/resources/ui_shape_builder.h>
#include "osm_service.hpp"

CommandScriptOSMManager::CommandScriptOSMManager( OSMManager& _hm ) {
    addCommandDefinition("load map", std::bind(&OSMManager::cmdLoadMap, &_hm, std::placeholders::_1 ) );
}

bool OSMBuilder::make( uint8_p&& _data, [[maybe_unused]] const DependencyStatus _status ) {

    osmArea = std::make_shared<OsmArea>(_data);
    notify(*this, Name());

    return true;
}

const std::shared_ptr<OsmArea>& OSMBuilder::getOSMArea() const {
    return osmArea;
}

void OSMManager::cmdLoadMap( const std::vector<std::string>& _params ) {
    OSMBuilder{ _params[0] }.build(getPtr());
}

void OSMManager::notified( OSMBuilder& _source, const std::string& generator ) {

    bsdata = _source.getOSMArea();
    bounds = Rect2f( bsdata->minLongitude, bsdata->minLatitude, bsdata->maxLongitude, bsdata->maxLatitude );
    //for ( const auto& node : bsdata->nodes ) vnodesMap[node.id] = &node;
    HeightScale( 5000.0f );
    medianLatitude = lerp( 0.5f, bsdata->maxLatitude, bsdata->minLatitude );
    latitudeScale = medianLatitude / 90.0f;

    auto tg = terrainGrid(V3fc::ZERO);
//    drawTriangles(RL.VPListPre3d(), std::get<0>(terrainGrid), std::get<1>(terrainGrid), C4fc::PASTEL_GRAY, "");
//
    for ( auto& rl : roadLines( V3fc::ZERO ) ) {
        UISB{ UIShapeType::Line3d }.v(std::get<0>(rl)).build(sg.RR());
    	//drawLine(RL.VPListPre3d(), std::get<0>(rl), std::get<2>(rl) / 255.0f, std::get<1>(rl) / 1000.0f, false);
    }
//
//    auto buildings = mOSM->buildings();
//    for (auto& b : buildings) {
//        auto bb = std::get<0>(b);
//        float raise = std::get<1>(b);
//        GeomBuilder{ bb, raise/1000.0f  }.m("white.jpg").build();
//    }

}

//std::shared_ptr<OpenStreetMap> OSMBuilder::build() {
//
//    auto elem = ServiceFactory::create<OpenStreetMap>();
//
//    elem->bsdata = ServiceFactory::create<OsmArea>( filename );
//    elem->bounds = Rect2f( elem->bsdata->minLongitude, elem->bsdata->minLatitude, elem->bsdata->maxLongitude,
//                           elem->bsdata->maxLatitude );
//    //for ( const auto& node : elem->bsdata->nodes ) elem->vnodesMap[node.id] = &node;
//    elem->HeightScale( 5000.0f );
//    elem->medianLatitude = lerp( 0.5f, elem->bsdata->maxLatitude, elem->bsdata->minLatitude );
//    elem->latitudeScale = elem->medianLatitude / 90.0f;
//
//    return elem;
//}

Vector3f OSMManager::get3dLonLat( const Vector2f& longitudeLatitude, const Vector3f& lcenter ) {
    return get3DCoord( longitudeLatitude, 0.0f, lcenter );
}

Vector3f OSMManager::get3dLatLon( const Vector2f& longitudeLatitude, const Vector3f& lcenter ) {
    return get3DCoord( { longitudeLatitude.y(), longitudeLatitude.x() }, 0.0f, lcenter );
}

Vector3f OSMManager::get3DCoord( const float longitude, const float latitude, const float elevation,
                                    const Vector3f& lcenter ) {
    return get3DCoord( { longitude, latitude }, elevation, lcenter );
}

Vector3f
OSMManager::get3DCoord( const Vector2f& latitudeLongitude, const float elevation, const Vector3f& lcenter ) {
    Vector2f point2d = bounds.normalizeWithinRect( latitudeLongitude ) * Vector2f( 1.0f, latitudeScale );
    return Vector3f{ point2d - Vector2f{ 0.5f, 0.5f * latitudeScale }, elevation / HeightScale() } + lcenter;
}

std::vector<std::tuple<std::vector<Vector3f>, float, Color4f>> OSMManager::roadLines( const Vector3f& lcenter ) {

    std::vector<std::tuple<std::vector<Vector3f>, float, Color4f>> result;

    for ( const auto& way : bsdata->highways ) {
        std::vector<Vector3f> linePoints;
        float width;
        Color4f color;
        for ( const auto& point : way.points ) {
            Vector3f point3d = get3DCoord( point.longitude, point.latitude, point.elevation, lcenter );
            linePoints.push_back( point3d );
        }
        width = way.width;
        color = way.color;
        std::tuple<std::vector<Vector3f>, float, Color4f> highwayInfo{ linePoints, width, color };

        result.push_back( highwayInfo );
    }

    return result;
}

std::vector<std::tuple<std::vector<Vector3f>, float>> OSMManager::buildings() {

    std::vector<std::tuple<std::vector<Vector3f>, float>> result;

    for ( const auto& building : bsdata->buildings ) {
        for ( const auto& buildingPart : building.parts ) {
            std::vector<Vector3f> basePoints;
            float height;

            for ( const auto& point : reverse(buildingPart.outer.points) ) {
                Vector3f point3d = get3DCoord( point.longitude, point.latitude, point.elevation+buildingPart.baseHeight,
                                               V3fc::ZERO );
                basePoints.push_back( point3d );
            }
            //basePoints.pop_back();

			height = buildingPart.topHeight;
            std::tuple<std::vector<Vector3f>, float> buildingInfo{ basePoints, height };

            result.push_back( buildingInfo );
        }
    }

    return result;
}


std::tuple<std::vector<Vector3f>, std::vector<int32_t>> OSMManager::terrainGrid( const Vector3f& lcenter ) {
    std::vector<Vector3f> verts;
    std::vector<int32_t> indices;
    uint32_t prevRowIndex;
    uint32_t currentRowIndex;

    //Create 3d points vector
    for ( uint32_t gridPointIndex = 0; gridPointIndex < bsdata->terrainGrid.grid.size(); gridPointIndex += 3 ) {
        verts.push_back(
                get3DCoord( bsdata->terrainGrid.grid[gridPointIndex + 1], bsdata->terrainGrid.grid[gridPointIndex + 0],
                            bsdata->terrainGrid.grid[gridPointIndex + 2] - 20.0f, lcenter ));
    }


    prevRowIndex = 0;

    for ( uint32_t row = 1; row < bsdata->terrainGrid.rows; row++ ) {

        currentRowIndex = prevRowIndex + static_cast<int32_t>(bsdata->terrainGrid.columns);

        for ( uint32_t col = 1; col < bsdata->terrainGrid.columns; col++ ) {

            uint32_t pointAIndex, pointBIndex, pointCIndex, pointDIndex;

            pointAIndex = prevRowIndex + (( col - 1 ));
            pointBIndex = prevRowIndex + ( col );
            pointCIndex = currentRowIndex + (( col - 1 ));
            pointDIndex = currentRowIndex + ( col );

            indices.push_back( pointAIndex );
            indices.push_back( pointCIndex );
            indices.push_back( pointBIndex );

            indices.push_back( pointBIndex );
            indices.push_back( pointCIndex );
            indices.push_back( pointDIndex );
        }

        prevRowIndex = currentRowIndex;
    }

    return std::tuple<std::vector<Vector3f>, std::vector<int32_t>> { verts, indices };
}
