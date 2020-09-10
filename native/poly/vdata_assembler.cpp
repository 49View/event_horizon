//
// Created by Dado on 2018-12-11.
//

#include "vdata_assembler.h"
#include <core/resources/profile.hpp>
#include <core/TTF.h>
#include <core/raw_image.h>
#include <core/math/poly_shapes.hpp>
#include <poly/poly_services.hpp>
#include <poly/follower.hpp>
#include <poly/converters/svg/svgtopoly.hpp>
#include <poly/converters/gltf2/gltf2.h>
#include <poly/scene_graph.h>

void internalCheckPolyNormal( Vector3f& ln, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ReverseFlag rf ) {
    if ( ln == V3fc::ZERO ) {
        ln = normalize( crossProduct( v1, v2, v3 ));
        if ( rf == ReverseFlag::True ) ln *= -1.0f;
    }
}

namespace VDataServices {

// ___ SHAPE BUILDER ___

    bool prepare( SceneGraph& sg, GT::Shape& _d, Material* matPtr ) {
        return true;
    }

    void buildInternal( const GT::Shape& _d, std::shared_ptr<VData> _ret ) {
        V3f center = V3fc::ZERO;
        V3f size = V3fc::ONE;
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
            case ShapeType::AABB:
                ps = createGeomForAABB( _d.aabb );
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
        auto ret = shapeTypeToString( _d.shapeType );
        switch ( _d.shapeType ) {
            case ShapeType::Cylinder:
            case ShapeType::Sphere:
            case ShapeType::Cube:
            case ShapeType::Panel:
            case ShapeType::Pillow:
            case ShapeType::RoundedCube:
            case ShapeType::None:
            case ShapeType::Pyramid:
            case ShapeType::Arrow:
                break;
            case ShapeType::AABB:
                ret += _d.aabb.minPoint().toString() + _d.aabb.maxPoint().toString();
                break;
        }

        return ret;
    }

// ___ POLY BUILDER ___

    bool prepare( SceneGraph& sg, GT::Poly& _d, Material* matPtr ) {
        _d.mappingData.fuvScale *= matPtr->getTexelRatio();
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
            if ( !_d.sourcePolylines2d.empty()) {
                std::vector<Vector3f> pl3{};
                for ( const auto& pl : _d.sourcePolylines2d ) {
                    _d.polyLines.emplace_back( PolyLine{ XZY::C(pl.verts), ln, _d.rfPoly } );
                }
            }
        }
        return true;
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
        auto ossString = oss.str();
        c.insert( std::end( c ), std::begin( ossString), std::end( ossString));
        return "Poly--" + Hashable<>::hashOf( c );
    }

    // ___ EXTRUDER BUILDER ___

    bool prepare( SceneGraph& sg, GT::Extrude& _d, Material* matPtr ) {
        bool hasData = false;
        _d.mappingData.fuvScale *= matPtr->getTexelRatio();
        if ( !_d.extrusionVerts.empty() ) {
            for ( const auto& po : _d.extrusionVerts ) {
                hasData |= !po.verts.empty();
            }
        }
        return hasData;
    }

    void buildInternal( const GT::Extrude& _d, std::shared_ptr<VData> _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( auto& ot : _d.extrusionVerts ) {
            auto evt = forceWindingOrder(ot.verts, ot.normal, WindingOrder::CCW);
            PolyServices::pull( _ret, evt, ot.normal, ot.zPull, dmProgressive ); //pullFlags
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
        auto ossString = oss.str();
        c.insert( std::end( c ), std::begin( ossString), std::end( ossString));
        return "Extrude--" + Hashable<>::hashOf( c );
    }

    // ___ Font BUILDER ___

    bool prepare( SceneGraph& sg, GT::Text& _d ) {
        if ( !_d.text.empty() ) {
            _d.font = sg.FM().get( _d.fontName );
            return true;
        }
        return false;
    }

    void buildInternal( const GT::Text& _d, std::shared_ptr<VData> _ret ) {
        float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
        Vector2f cursor = V2fc::ZERO;
        Topology mesh;
        size_t q = 0;

        for ( char i : _d.text ) {
            Utility::TTF::CodePoint cp( static_cast<Utility::TTFCore::ulong>(i));
            const Utility::TTF::Mesh& m = _d.font->GetTriangulation( cp );

            // Don't draw an empty space
            if ( !m.verts.empty() ) {
                for ( size_t t = 0; t < m.verts.size(); t+=3 ) {
                    Vector2f p1{ m.verts[t].pos.x / gliphScaler  , 1.0f - ( m.verts[t].pos.y / gliphScaler ) };
                    Vector2f p3{ m.verts[t+2].pos.x / gliphScaler, 1.0f - ( m.verts[t+2].pos.y / gliphScaler ) };
                    Vector2f p2{ m.verts[t+1].pos.x / gliphScaler, 1.0f - ( m.verts[t+1].pos.y / gliphScaler ) };
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

        PolyStruct ps = createGeom( mesh, V3fc::ONE, GeomMappingT::Planar );
        _ret->fill( ps );
        _ret->BBox3d( ps.bbox3d );
    }

    ResourceRef refName( const GT::Text& _d ) {
        auto c = _d.serialize();
        return "Text--" + Hashable<>::hashOf( c );
    }

    // ___ QUAD MESH BUILDER ___

    bool prepare( SceneGraph& sg, GT::Mesh& _d, Material* matPtr ) {
        _d.mappingData.fuvScale *= matPtr->getTexelRatio();
        return !_d.quads.empty();
    }

    void buildInternal( const GT::Mesh& _d, std::shared_ptr<VData> _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( const auto& q : _d.quads ) {
            PolyServices::addFlatPoly( _ret, q.quad, q.normal, dmProgressive );
        }
    }

    ResourceRef refName( const GT::Mesh& _d ) {
        std::stringstream oss;
        for ( const auto& q : _d.quads ) {
            oss << q.normal.toString();
            for ( const auto& v : q.quad ) {
                oss << v.toString();
            }
        }
        return "Quad--" + Hashable<>::hashOf(oss.str());
    }

    // ___ CLOTH MESH BUILDER ___

    bool prepare( SceneGraph& sg, GT::ClothMesh& _d, Material* matPtr ) {
        _d.mappingData.fuvScale *= matPtr->getTexelRatio();
        return true;
    }

    void buildInternal( const GT::ClothMesh& _d, std::shared_ptr<VData> _ret ) {
        if ( _d.cloth ) {
            Topology mesh;

            auto ph = _d.cloth->getParticleHeight();
            auto pw = _d.cloth->getParticleWidth();
            for (int y = 0; y < ph; y++) {
                for (int x = 0; x < pw; x++) {
                    glm::vec3 vy, vx;
                    Particle *p = &_d.cloth->particles[y*pw+x];

                    if (y > 0) vy = _d.cloth->particles[(y-1)*pw+x].pos;
                    else vy = p->pos;
                    if (y < ph) vy -= _d.cloth->particles[(ph-1)*pw+x].pos;
                    else vy -= p->pos;

                    if (x > 0) vx = _d.cloth->particles[y*pw+x-1].pos;
                    else vx = p->pos;
                    if (x < pw-1) vx -= _d.cloth->particles[y*pw+x+1].pos;
                    else vx -= p->pos;

                    p->normal = glm::normalize(glm::cross(vy, vx));
                }
            }

            int q = 0;
            for ( const auto tri : _d.cloth->Triangles() ) {
                mesh.vertices.emplace_back( V3f{ tri.particles[0]->pos.x, tri.particles[0]->pos.y, tri.particles[0]->pos.z}*0.1f );
                mesh.vertices.emplace_back( V3f{ tri.particles[2]->pos.x, tri.particles[2]->pos.y, tri.particles[2]->pos.z}*0.1f );
                mesh.vertices.emplace_back( V3f{ tri.particles[1]->pos.x, tri.particles[1]->pos.y, tri.particles[1]->pos.z}*0.1f );
                mesh.addTriangle( q, q+1, q+2 );
                q+=3;
            }

            PolyStruct ps = createGeom( mesh, V3fc::ONE, GeomMapping{ GeomMappingT::Cube, V3fc::ONE*8.0f}, 0, _d.rfPoly );
            _ret->fill( ps );
            _ret->BBox3d( ps.bbox3d );
        }
    }

    ResourceRef refName( const GT::ClothMesh& _d ) {
        std::stringstream oss;
        oss << _d.cloth->getParticleHeight() << _d.cloth->getParticleWidth();
        return "Cloth--" + oss.str();
    }

// ___ FOLLOWER BUILDER ___

    bool prepare( SceneGraph& sg, GT::Follower& _d, Material* matPtr ) {
        _d.mappingData.fuvScale *= matPtr->getTexelRatio();
        if ( _d.profilePath.empty() && !_d.profilePath2d.empty() ) {
            for (auto &v: _d.profilePath2d) _d.profilePath.emplace_back( Vector3f{v, _d.z} );
        }
        return true;
    }

    void buildInternal( const GT::Follower& _d, std::shared_ptr<VData> _ret ) {
        ASSERT( !_d.profile->Points().empty() );

        Profile lProfile{ *_d.profile.get() };
        Vector2f lRaise{V2fc::ZERO};
        if ( _d.fraise != PolyRaise::None ) {
            switch ( _d.fraise ) {
                case PolyRaise::None:break;
                case PolyRaise::HorizontalPos:
                    lRaise= ( V2fc::X_AXIS * lProfile.width() );
                    break;
                case PolyRaise::HorizontalNeg:
                    lRaise= ( V2fc::X_AXIS_NEG * lProfile.width());
                    break;
                case PolyRaise::VerticalPos:
                    lRaise= ( V2fc::Y_AXIS * lProfile.height() );
                    break;
                case PolyRaise::VerticalNeg:
                    lRaise= ( V2fc::Y_AXIS_NEG * lProfile.height() );
                    break;
            };
        }

        lProfile.raise( lRaise );
        lProfile.flip( _d.flipVector );

        FollowerService::extrude( _ret, _d.profilePath, lProfile, _d.mFollowerSuggestedAxis, _d.fflags );
    }

    ResourceRef refName( const GT::Follower& _d ) {
        std::stringstream oss;
        oss << _d.profile->Name();
        for ( const auto& v : _d.profilePath ) {
            oss << v.toString();
        }
        oss << _d.fflags;
        oss << static_cast<uint64_t>(_d.fraise);
        oss << _d.flipVector.toString();
        // ### Implement this for gaps
//    for ( const auto& v : mGaps ) {
//        oss << static_cast<uint64_t>(v);
//    }
        oss << _d.mFollowerSuggestedAxis.toString();
       return "Follower--" + Hashable<>::hashOf(oss.str());
    }

    // ___ GLTF2 BUILDER ___

    bool prepare( SceneGraph& sg, GT::GLTF2& _d, Material* matPtr ) {
        return true;
    }

    void buildInternal( const GT::GLTF2& _d, std::shared_ptr<VData> _ret ) {
        GLTF2Service::fillGeom( _ret, _d.model, _d.meshIndex, _d.primitiveIndex );
    }

    ResourceRef refName( const GT::GLTF2& _d ) {
        std::stringstream oss;
        oss << _d.nameId << _d.meshIndex <<  _d.primitiveIndex;
        return "GLTF2--" + oss.str();
    }

    // ___ ASSET BUILDER ___

    bool prepare( SceneGraph& sg, GT::Asset& _d, Material* matPtr ) {
        return true;
    }

    void buildInternal( const GT::Asset& _d, std::shared_ptr<VData> _ret ) {

    }

    ResourceRef refName( const GT::Asset& _d ) {
        std::stringstream oss;
        oss << _d.nameId;
        return "Asset--" + oss.str();
    }

// ********************************************************************************************************************
// ********************************************************************************************************************
//
// ___ OSM BUILDER ___
//
// ********************************************************************************************************************
// ********************************************************************************************************************

    bool prepare( SceneGraph& sg, GT::OSM& _d, Material *matPtr ) {
        return true;
    }


    V2f coordToProjection( const V2f& latLon ) {
        return V2f{ latLon.x(), radToDeg(log(tan((latLon.y() / 90.0f + 1.0f) * M_PI_4 )))};
    }

    template <typename T>
    T latToY( T lat ) {
        return radToDeg(log(tan((lat / 90.0 + 1.0) * M_PI_4 )));
    }

    template <typename T>
    float calcGeoDistance( T lat1, T lon1, T lat2, T lon2 ) {
        auto R = 6371e3; // metres
        auto phi1 = degToRad(lat1); // φ, λ in radians
        auto phi2 = degToRad(lat2);
        auto deltaPhi = degToRad(lat2-lat1);
        auto deltaLambda = degToRad(lon2-lon1);

        auto a = sin(deltaPhi/2.0) * sin(deltaPhi/2.0) +
                  cos(phi1) * cos(phi2) *
                  sin(deltaLambda/2.0) * sin(deltaLambda/2.0);
        auto c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));

        auto d = (R * c); // in metres

        return d;
    }

    void buildInternal( const GT::OSM& _d, std::shared_ptr<VData> _ret ) {

        Topology mesh{};
        std::vector<C4f> vertexColors;

        V2f centerProj = coordToProjection(_d.locationLatLon);
        size_t indexOffset = 0;

        for ( const auto& element : _d.osmData.elements ) {
            V2f elemLatLon = V2f{element.center.lon,element.center.lat};
            V2f elemCenterProj = coordToProjection(elemLatLon);// - centerProj;
            float coordDistance = calcGeoDistance( (double)_d.locationLatLon.x(), (double)latToY(_d.locationLatLon.y()), element.center.lon, latToY(element.center.lat));
            V2f elemCenterOffset = normalize( elemCenterProj - centerProj ) * coordDistance;
            V3f elemCenterProj3d = XZY::C( elemCenterOffset, 0.0f);
            for ( const auto& group : element.groups ) {
                C4f color = C4fc::XTORGBA(group.colour);
                for ( const auto& vertex : group.triangles ) {
                    V3f pp{XZY::C(vertex) + elemCenterProj3d};
                    pp.swizzle(0,2);
                    mesh.addVertex( pp );
                    vertexColors.emplace_back( color );
                }
                for ( auto ti = indexOffset; ti < indexOffset + group.triangles.size(); ti+=3  ) {
                    if ( !isCollinear(mesh.vertices[ti], mesh.vertices[ti+1], mesh.vertices[ti+2]) ) {
                        mesh.addTriangle(ti, ti+1, ti+2);
                    }
                }
                indexOffset += group.triangles.size();
            }
        }

        PolyStruct ps = createGeom( mesh, V3fc::ONE, GeomMapping{ GeomMappingT::Cube, V3fc::ONE}, 0, ReverseFlag::False, vertexColors );
        _ret->fill( ps );
        _ret->BBox3d( ps.bbox3d );
    }

    ResourceRef refName( const GT::OSM& _d ) {
        return ResourceRef();
    }

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

}
