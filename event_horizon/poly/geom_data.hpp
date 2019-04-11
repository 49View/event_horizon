//
//  geom_data.hpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#pragma once

#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <list>
#include <iostream>
#include <string>

#include <core/htypes_shared.hpp>
#include <core/math/poly_utils.hpp>
#include <core/math/poly_shapes.hpp>
#include <core/math/matrix4f.h>
#include <core/math/rect2f.h>
#include <core/math/plane3f.h>
#include <core/math/aabb.h>
#include <core/math/quad_vertices.h>
#include <core/soa_utils.h>
#include <core/boxable.hpp>
#include <core/name_policy.hpp>
#include <core/serialization.hpp>
#include <core/v_data.hpp>
#include <core/recursive_transformation.hpp>

#include <poly/polypartition.h>
#include <poly/poly_helper.h>

class Profile;
class GeomData;
class GeomBuilder;

class GeomData : public Boxable<JMATH::AABB>, public NamePolicy<> {
public:
    GeomData();
    virtual ~GeomData();

    GeomData( const QuadVector3fNormalfList& quads, const GeomMappingData& _mapping );
public:

    void checkBaricentricCoordsOn( const Vector3f& i, int32_t pIndexStart, int32_t pIndexEnd, int32_t& pIndex, float& u,
                                   float& v );

    void flipNormals();

    // Utils
    void doNotScaleMapping();
    static std::vector<Vector3f> utilGenerateFlatBoxFromRect( const JMATH::Rect2f& bbox, float z );
    static std::vector<Vector2f> utilGenerateFlatRect( const Vector2f& size, const WindingOrderT wo = WindingOrder::CCW,
                                                       PivotPointPosition ppp = PivotPointPosition::PPP_CENTER,
                                                       const Vector2f& pivot = Vector2f::ZERO );
    static std::vector<Vector3f> utilGenerateFlatBoxFromSize( float width, float height, float z );

    void Bevel( const Vector3f& bevelAmount );
    Vector3f Bevel() const;

    const VData& getVData() const;

    virtual void debugPrint();
    void setWindingOrderFlagOnly( WindingOrderT _wo );
    WindingOrderT getWindingOrder() const;

    void resetWrapMapping( const std::vector<float>& yWrapArray );
    void updateWrapMapping( Vector3f vs[4], Vector2f vtcs[4], uint64_t m, uint64_t size );

    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( "BBbox", mVdata.BBox3d() ); }

    // All internal add polygons are now not accessible to the outside to handle topology better

protected:

    // Mapping
    void setMappingData( const GeomMappingData& _mapping );
    void resetMapping( uint64_t arraySize );
    void propagateWrapMapping( const GeomData *source );



protected:
    VData mVdata;

    // Mapping
    GeomMappingData mapping;

};
