//
// Created by Dado on 2018-12-11.
//

#include "poly_helper.h"
#include <core/resources/profile.hpp>
#include <poly/polyclipping/clipper.hpp>
#include <poly/poly_services.hpp>
#include <poly/follower.hpp>

void clipperToPolylines( std::vector<PolyLine2d>& ret, const ClipperLib::Paths& solution,
                                            const Vector3f& _normal, ReverseFlag rf ) {

    for ( const auto& cp : solution ) {
        bool pathOrient = Orientation( cp );

        if ( pathOrient && cp.size() ) {
            std::vector<Vector2f> fpoints;
            fpoints.reserve(cp.size());
            for ( const auto& p : cp ) {
                fpoints.push_back( Vector2f{ static_cast<int>( p.X ), static_cast<int>( p.Y ) } * 0.001f );
            }
            ret.emplace_back( PolyLine2d{ fpoints, _normal, rf  } );
        }
    }

}

std::vector<PolyLine2d> clipperToPolylines( const ClipperLib::Paths& source, const ClipperLib::Path& clipAgainst,
                                            const Vector3f& _normal, ReverseFlag rf ) {
    std::vector<PolyLine2d> ret;
    for ( auto& sl : source ) {
        ClipperLib::Clipper c;
        ClipperLib::Paths solution;
        c.AddPath( sl, ClipperLib::ptSubject, true );
        c.AddPath( clipAgainst, ClipperLib::ptClip, true );
        c.Execute( ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero );

        clipperToPolylines( ret, solution, _normal, rf );
    }

    return ret;
}

ClipperLib::Path getPerimeterPath( const std::vector<Vector2f>& _values ) {
    ClipperLib::Path ret;
    for ( auto& p : _values ) {
        ret << p;
    }
    return ret;
}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ SHAPE BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

GeomDataShapeBuilder::GeomDataShapeBuilder( ShapeType shapeType ) : shapeType( shapeType ) {
    mRefName = shapeTypeToString( shapeType );
}

void GeomDataShapeBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
    V3f center = V3f::ZERO;
    V3f size = V3f::ONE;
    int subDivs = 3;
    PolyStruct ps;

    switch ( shapeType ) {
        case ShapeType::Cylinder:
            ps = createGeomForCylinder( center, size.xy(), subDivs );
            break;
        case ShapeType::Sphere:
            ps = createGeomForSphere( center, size.x(), subDivs );
            break;
        case ShapeType::Cube:
            ps = createGeomForCube( center, size );
            break;
        case ShapeType::Panel:
            ps = createGeomForPanel( center, size );
            break;
        case ShapeType::Pillow:
            ps = createGeomForPillow( center, size, subDivs );
            break;
        case ShapeType::RoundedCube:
            ps = createGeomForRoundedCube( center, size, subDivs );
            break;
        default:
            ps = createGeomForSphere( center, size.x(), subDivs );
    }

    _ret->fill(ps);
    _ret->BBox3d(ps.bbox3d);
}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ OUTLINE BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

void GeomDataOutlineBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
    for ( const auto& ot : outlineVerts ) {
        PolyServices::pull( _ret, ot.verts, ot.zPull, mappingData ); //pullFlags
    }
}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ POLY BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

void GeomDataPolyBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
    for ( const auto& poly : polyLine ) {
        PolyServices::addFlatPolyTriangulated( _ret, poly.verts.size(), poly.verts.data(), poly.normal, mappingData,
                                               static_cast<bool>(poly.reverseFlag) );
    }
}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ QUAD MESH BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

void GeomDataQuadMeshBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
    for ( const auto& q : quads ) {
        PolyServices::addFlatPoly( _ret, q.quad, q.normal, mappingData );
    }

}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ FOLLOWER BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

void GeomDataFollowerBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
    ASSERT( !mProfile->Points().empty() );

    Profile lProfile{ *mProfile.get() };
    Vector2f lRaise = mRaise;
    if ( mRaiseEnum != PolyRaise::None ) {
        switch ( mRaiseEnum ) {
            case PolyRaise::None:break;
            case PolyRaise::HorizontalPos:
                lRaise= ( Vector2f::X_AXIS * lProfile.width() );
                break;
            case PolyRaise::HorizontalNeg:
                lRaise= ( Vector2f::X_AXIS_NEG * lProfile.width());
                break;
            case PolyRaise::VerticalPos:
                lRaise= ( Vector2f::Y_AXIS * lProfile.height() );
                break;
            case PolyRaise::VerticalNeg:
                lRaise= ( Vector2f::Y_AXIS_NEG * lProfile.height() );
                break;
        };
    }

    lProfile.raise( lRaise );
    lProfile.flip( mFlipVector );

    _ret = FollowerService::extrude( mVerts, lProfile, mSuggestedAxis, followersFlags );
}
