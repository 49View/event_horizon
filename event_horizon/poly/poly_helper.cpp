//
// Created by Dado on 2018-12-11.
//

#include "poly_helper.h"
#include <poly/polyclipping/clipper.hpp>


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
