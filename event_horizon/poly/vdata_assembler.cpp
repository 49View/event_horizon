//
// Created by Dado on 2018-12-11.
//

#include "vdata_assembler.h"
#include <core/resources/profile.hpp>
#include <core/TTF.h>
#include <core/math/poly_shapes.hpp>
#include <poly/polyclipping/clipper.hpp>
#include <poly/poly_services.hpp>
#include <poly/follower.hpp>
#include <poly/converters/svg/svgtopoly.hpp>
#include <poly/scene_graph.h>

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

void internalCheckPolyNormal( Vector3f& ln, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ReverseFlag rf ) {
    if ( ln == Vector3f::ZERO ) {
        ln = normalize( crossProduct( v1, v2, v3 ));
        if ( rf == ReverseFlag::True ) ln *= -1.0f;
    }
}

namespace VDataServices {

// ___ SHAPE BUILDER ___

    void prepare( SceneGraph& sg, GT::Shape& _d ) {}

    void buildInternal( const GT::Shape& _d, std::shared_ptr<VData> _ret ) {
        V3f center = V3f::ZERO;
        V3f size = V3f::ONE;
        int subDivs = 3;
        PolyStruct ps;

        switch ( _d.shapeType ) {
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

        _ret->fill( ps );
        _ret->BBox3d( ps.bbox3d );
    }

    ResourceRef refName( const GT::Shape& _d ) {
        return shapeTypeToString( _d.shapeType );
    }

// ___ POLY BUILDER ___

    void prepare( SceneGraph& sg, GT::Poly& _d ) {
        if ( _d.mappingData.bDoNotScaleMapping ) MappingServices::doNotScaleMapping( _d.mappingData );
        if ( _d.polyLines.empty()) {
            Vector3f ln = _d.forcingNormalPoly;
            if ( !_d.sourcePolysTris.empty()) {
                auto[v1, v2, v3] = _d.sourcePolysTris[0];
                internalCheckPolyNormal( ln, v1, v2, v3, _d.rfPoly );
                for ( const auto& tri : _d.sourcePolysTris ) {
                    _d.polyLines.emplace_back( PolyLine{ tri, ln, _d.rfPoly } );
                }
            }
            if ( !_d.sourcePolysVList.empty()) {
                internalCheckPolyNormal( ln, _d.sourcePolysVList.at( 0 ), _d.sourcePolysVList.at( 1 ),
                                         _d.sourcePolysVList.at( 2 ), _d.rfPoly );
                _d.polyLines.emplace_back( PolyLine{ _d.sourcePolysVList, ln, _d.rfPoly } );
            }
        }
    }

    void buildInternal( const GT::Poly& _d, std::shared_ptr<VData> _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( const auto& poly : _d.polyLines ) {
            PolyServices::addFlatPolyTriangulated( _ret, poly.verts.size(), poly.verts.data(), poly.normal,
                                                   dmProgressive,
                                                   static_cast<bool>(poly.reverseFlag));
        }
    }

    ResourceRef refName( const GT::Poly& _d ) {
        std::stringstream oss;
        for ( const auto& poly : _d.polyLines ) {
            for ( const auto& v : poly.verts ) {
                oss << v.toString();
            }
            oss << poly.normal.toString();
            oss << static_cast<uint64_t>(poly.reverseFlag);
        }
        auto c = _d.mappingData.serialize();
        c.insert( std::end( c ), std::begin( oss.str()), std::end( oss.str()));
        return "Poly--" + Hashable<>::hashOf( c );
    }

    // ___ EXTRUDER BUILDER ___

    void prepare( SceneGraph& sg, GT::Extrude& _d ) {}

    void buildInternal( const GT::Extrude& _d, std::shared_ptr<VData> _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( auto& ot : _d.extrusionVerts ) {
            PolyServices::pull( _ret, ot.verts, ot.zPull, dmProgressive ); //pullFlags
        }
    }

    ResourceRef refName( const GT::Extrude& _d ) {
        std::stringstream oss;
        for ( const auto& ot : _d.extrusionVerts ) {
            for ( const auto& v : ot.verts ) {
                oss << v.toString();
            }
            oss << ot.zPull;
        }
        auto c = _d.mappingData.serialize();
        c.insert( std::end( c ), std::begin( oss.str()), std::end( oss.str()));
        return "Extrude--" + Hashable<>::hashOf( c );
    }

    // ___ Font BUILDER ___

    void prepare( SceneGraph& sg, GT::Text& _d ) {
        _d.font = sg.FM().get( _d.fontName );
        if ( _d.text.empty() ) {
            _d.text = ".";
        }
    }

    void buildInternal( const GT::Text& _d, std::shared_ptr<VData> _ret ) {
        float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
        Vector2f cursor = Vector2f::ZERO;
        Topology mesh;
        size_t q = 0;

        for ( char i : _d.text ) {
            Utility::TTF::CodePoint cp( static_cast<Utility::TTFCore::ulong>(i));
            const Utility::TTF::Mesh& m = _d.font->GetTriangulation( cp );

            // Don't draw an empty space
            if ( !m.verts.empty() ) {
                for ( size_t t = 0; t < m.verts.size(); t+=3 ) {
                    Vector2f p1{ m.verts[t].pos.x / gliphScaler  , 1.0f - ( m.verts[t].pos.y / gliphScaler ) };
                    Vector2f p3{ m.verts[t+1].pos.x / gliphScaler, 1.0f - ( m.verts[t+1].pos.y / gliphScaler ) };
                    Vector2f p2{ m.verts[t+2].pos.x / gliphScaler, 1.0f - ( m.verts[t+2].pos.y / gliphScaler ) };
                    mesh.vertices.emplace_back( XZY::C( p1 + cursor ) );
                    mesh.vertices.emplace_back( XZY::C( p2 + cursor ) );
                    mesh.vertices.emplace_back( XZY::C( p3 + cursor ) );
                    mesh.addTriangle( q, q+1, q+2 );
                    q+=3;
//                    fs->addVertex( XZY::C( pos4.xyz()), Vector2f{ m.verts[t].texCoord, m.verts[t].coef } );
                }
            }

            Utility::TTFCore::vec2f kerning = _d.font->GetKerning( Utility::TTF::CodePoint( i ), cp );
            Vector2f nextCharPos = Vector2f( kerning.x / gliphScaler, kerning.y / gliphScaler );
            cursor += nextCharPos;
        }

        PolyStruct ps = createGeom( mesh, V3f::ONE, GeomMapping::Planar );
        _ret->fill( ps );
        _ret->BBox3d( ps.bbox3d );
    }

    ResourceRef refName( const GT::Text& _d ) {
        std::stringstream oss;
        auto c = _d.serialize();
        c.insert( std::end( c ), std::begin( oss.str()), std::end( oss.str()));
        return "Text--" + Hashable<>::hashOf( c );
    }

}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ QUAD MESH BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

//void GeomDataQuadMeshBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
//    for ( const auto& q : quads ) {
//        PolyServices::addFlatPoly( _ret, q.quad, q.normal, mappingData );
//    }
//}
//
//void GeomDataQuadMeshBuilder::setupRefName() {
//    std::stringstream oss;
//    for ( const auto& q : quads ) {
//        oss << q.normal.toString();
//        for ( const auto& v : q.quad ) {
//            oss << v.toString();
//        }
//    }
//    mRefName = "Poly--" + Hashable<>::hashOf(oss.str());
//}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ FOLLOWER BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

//void GeomDataFollowerBuilder::buildInternal( std::shared_ptr<VData> _ret ) {
//    ASSERT( !mProfile->Points().empty() );
//
//    Profile lProfile{ *mProfile.get() };
//    Vector2f lRaise = mRaise;
//    if ( mRaiseEnum != PolyRaise::None ) {
//        switch ( mRaiseEnum ) {
//            case PolyRaise::None:break;
//            case PolyRaise::HorizontalPos:
//                lRaise= ( Vector2f::X_AXIS * lProfile.width() );
//                break;
//            case PolyRaise::HorizontalNeg:
//                lRaise= ( Vector2f::X_AXIS_NEG * lProfile.width());
//                break;
//            case PolyRaise::VerticalPos:
//                lRaise= ( Vector2f::Y_AXIS * lProfile.height() );
//                break;
//            case PolyRaise::VerticalNeg:
//                lRaise= ( Vector2f::Y_AXIS_NEG * lProfile.height() );
//                break;
//        };
//    }
//
//    lProfile.raise( lRaise );
//    lProfile.flip( mFlipVector );
//
//    FollowerService::extrude( _ret, mVerts, lProfile, mSuggestedAxis, followersFlags );
//}
//
//void GeomDataFollowerBuilder::setupRefName() {
//    std::stringstream oss;
//    oss << mProfile->Name();
//    for ( const auto& v : mVerts ) {
//        oss << v.toString();
//    }
//    oss << followersFlags;
//    oss << mRaise.toString();
//    oss << static_cast<uint64_t>(mRaiseEnum);
//    oss << mFlipVector.toString();
//    // ### Implement this for gaps
////    for ( const auto& v : mGaps ) {
////        oss << static_cast<uint64_t>(v);
////    }
//    oss << mSuggestedAxis.toString();
//    mRefName = "Follower--" + Hashable<>::hashOf(oss.str());
//}

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ SVG BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

//GeomDataListBuilderRetType GeomDataSVGBuilder::build() {
//    auto rawPoints = SVGC::SVGToPoly( svgAscii );
//
//    GeomDataListBuilderRetType logoGeoms{};
//    logoGeoms.reserve( rawPoints.size() );
//    for ( const auto& points : rawPoints ) {
//        auto fb = std::make_shared<GeomDataFollowerBuilder>( mProfile,
//                                                             XZY::C(points.path,0.0f),
//                                                             FollowerFlags::WrapPath );
////        _mat->c( points.strokeColor );
//        logoGeoms.emplace_back(fb->build());
//    }
//    return logoGeoms;
//}
