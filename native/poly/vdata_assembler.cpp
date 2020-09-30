//
// Created by Dado on 2018-12-11.
//

#include "vdata_assembler.h"
#include <core/resources/profile.hpp>
#include <core/TTF.h>
#include <core/math/obb.hpp>
#include <core/math/poly_shapes.hpp>
#include <poly/poly_services.hpp>
#include <poly/follower.hpp>
#include <poly/converters/svg/svgtopoly.hpp>
#include <utility>
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

    bool prepare( SceneGraph& sg, GT::Shape& _d, Material* ) {
        return true;
    }

    [[maybe_unused]] void buildInternal( const GT::Shape& _d, const std::shared_ptr<VData>& _ret ) {
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
    }

    ResourceRef refName( const GT::Shape& _d ) {
        auto ret = shapeTypeToString( _d.shapeType );
        ret += UUIDGen::make();
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

    [[maybe_unused]] void buildInternal( const GT::Poly& _d, const std::shared_ptr<VData>& _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( const auto& poly : _d.polyLines ) {
            PolyServices::addFlatPolyTriangulated( _ret, poly.verts.size(), poly.verts.data(), poly.normal,
                                                   dmProgressive,
                                                   static_cast<bool>(poly.reverseFlag));
        }
    }

    ResourceRef refName( const GT::Poly& _d ) {
//        std::stringstream oss;
//        for ( const auto& poly : _d.polyLines ) {
//            for ( const auto& v : poly.verts ) {
//                oss << v.toString();
//            }
//            oss << poly.normal.toString();
//            oss << static_cast<uint64_t>(poly.reverseFlag);
//        }
//        auto c = _d.mappingData.serialize();
//        auto ossString = oss.str();
//        c.insert( std::end( c ), std::begin( ossString), std::end( ossString));
//        return "Poly--" + Hashable<>::hashOf( c );
        return "Poly--" + UUIDGen::make();
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

    [[maybe_unused]] void buildInternal( const GT::Extrude& _d, const std::shared_ptr<VData>& _ret ) {
        auto dmProgressive = _d.mappingData;
        for ( auto& ot : _d.extrusionVerts ) {
            auto evt = forceWindingOrder(ot.verts, ot.normal, WindingOrder::CCW);
            PolyServices::pull( _ret, evt, ot.normal, ot.zPull, dmProgressive ); //pullFlags
        }
    }

    ResourceRef refName( const GT::Extrude& _d ) {
//        std::stringstream oss;
//        for ( const auto& ot : _d.extrusionVerts ) {
//            for ( const auto& v : ot.verts ) {
//                oss << v.toString();
//            }
//            oss << ot.zPull;
//        }
//        auto c = _d.mappingData.serialize();
//        auto ossString = oss.str();
//        c.insert( std::end( c ), std::begin( ossString), std::end( ossString));
//        return "Extrude--" + Hashable<>::hashOf( c );
        return "Extrude--" + UUIDGen::make();
    }

    // ___ Font BUILDER ___

    bool prepare( SceneGraph& sg, GT::Text& _d ) {
        if ( !_d.text.empty() ) {
            _d.font = sg.FM().get( _d.fontName );
            return true;
        }
        return false;
    }

    [[maybe_unused]] void buildInternal( const GT::Text& _d, const std::shared_ptr<VData>& _ret ) {
        float glyphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
        Vector2f cursor = V2fc::ZERO;
        Topology mesh;
        size_t q = 0;

        for ( char i : _d.text ) {
            Utility::TTF::CodePoint cp( static_cast<Utility::TTFCore::ulong>(i));
            const Utility::TTF::Mesh& m = _d.font->GetTriangulation( cp );

            // Don't draw an empty space
            if ( !m.verts.empty() ) {
                for ( size_t t = 0; t < m.verts.size(); t+=3 ) {
                    Vector2f p1{ m.verts[t].pos.x / glyphScaler  , 1.0f - ( m.verts[t].pos.y / glyphScaler ) };
                    Vector2f p3{ m.verts[t+2].pos.x / glyphScaler, 1.0f - ( m.verts[t + 2].pos.y / glyphScaler ) };
                    Vector2f p2{ m.verts[t+1].pos.x / glyphScaler, 1.0f - ( m.verts[t + 1].pos.y / glyphScaler ) };
                    mesh.vertices.emplace_back( XZY::C( p1 + cursor ) );
                    mesh.vertices.emplace_back( XZY::C( p2 + cursor ) );
                    mesh.vertices.emplace_back( XZY::C( p3 + cursor ) );
                    mesh.addTriangle( q, q+1, q+2 );
                    q+=3;
                }
            }

            Utility::TTFCore::vec2f kerning = _d.font->GetKerning( Utility::TTF::CodePoint( i ), cp );
            Vector2f nextCharPos = Vector2f(kerning.x / glyphScaler, kerning.y / glyphScaler );
            cursor += nextCharPos;
        }

        PolyStruct ps = createGeom( mesh, V3fc::ONE, GeomMappingT::Planar );
        _ret->fill( ps );
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

    [[maybe_unused]] void buildInternal( const GT::Mesh& _d, const std::shared_ptr<VData>& _ret ) {
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

    [[maybe_unused]] void buildInternal( const GT::ClothMesh& _d, const std::shared_ptr<VData>& _ret ) {
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

    [[maybe_unused]] void buildInternal( const GT::Follower& _d, std::shared_ptr<VData> _ret ) {
        ASSERT( !_d.profile->Points().empty() );

        Profile lProfile{ *_d.profile };
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
            }
        }

        lProfile.raise( lRaise );
        lProfile.flip( _d.flipVector );

        FollowerService::extrude( std::move(_ret), _d.profilePath, lProfile, _d.mFollowerSuggestedAxis, _d.fflags );
    }

    ResourceRef refName( const GT::Follower& _d ) {
//        std::stringstream oss;
//        oss << _d.profile->Name();
//        for ( const auto& v : _d.profilePath ) {
//            oss << v.toString();
//        }
//        oss << _d.fflags;
//        oss << static_cast<uint64_t>(_d.fraise);
//        oss << _d.flipVector.toString();
//        // ### Implement this for gaps
////    for ( const auto& v : mGaps ) {
////        oss << static_cast<uint64_t>(v);
////    }
//        oss << _d.mFollowerSuggestedAxis.toString();
//       return "Follower--" + Hashable<>::hashOf(oss.str());
       return "Follower--" + UUIDGen::make(); //Hashable<>::hashOf(oss.str());
    }

    // ___ GLTF2 BUILDER ___

    bool prepare( SceneGraph& sg, GT::GLTF2& _d, Material* ) {
        return true;
    }

    [[maybe_unused]] void buildInternal( const GT::GLTF2& _d, std::shared_ptr<VData> _ret ) {
        GLTF2Service::fillGeom( std::move(_ret), _d.model, _d.meshIndex, _d.primitiveIndex );
    }

    ResourceRef refName( const GT::GLTF2& _d ) {
        std::stringstream oss;
        oss << _d.nameId << _d.meshIndex <<  _d.primitiveIndex;
        return "GLTF2--" + oss.str();
    }

    // ___ ASSET BUILDER ___

    bool prepare( SceneGraph& sg, GT::Asset& _d, Material* ) {
        return true;
    }

    void buildInternal( const GT::Asset& _d, [[maybe_unused]] const std::shared_ptr<VData>& _ret ) {

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

    bool prepare( SceneGraph& sg, GT::OSM& _d, Material* ) {
        return true;
    }


    V2f coordToProjection( const V2f& latLon ) {
        return V2f{ latLon.x(), radToDeg(log(tan((latLon.y() / 90.0f + 1.0f) * M_PI_4 )))};
    }

    [[maybe_unused]] void buildInternal( const GT::OSM& _d, const std::shared_ptr<VData>& _ret ) {

        Topology mesh{};

        size_t indexOffset = 0;
        float globalScale = 0.01f;
        float facadeMappingScale = 0.1f;

        double minX = std::numeric_limits<double>::max(), minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest(), maxY = std::numeric_limits<double>::lowest();

        for ( const auto& element : _d.osmData.elements ) {
            if ( element.center.x < minX ) minX = element.center.x;
            if ( element.center.y < minY ) minY = element.center.y;
            if ( element.center.x > maxX ) maxX = element.center.x;
            if ( element.center.y > maxY ) maxY = element.center.y;
        }
        double sizeX = maxX - minX;
        double sizeY = maxY - minY;
        for ( const auto& element : _d.osmData.elements ) {
            double elemCX = remap( element.center.x, minX, maxX, -sizeX*0.5, sizeX*0.5 );
            double elemCY = remap( element.center.y, minY, maxY, -sizeY*0.5, sizeY*0.5 );
            V3f elemCenterProj3d = XZY::C( V2f{elemCX, elemCY}, 0.0f);

            for ( const auto& group : element.groups ) {
                auto mp = [group, elemCenterProj3d, globalScale]( auto i ) -> V3f {
                    auto vertex = group.triangles[i];
                    V3f pp{XZY::C(vertex) + elemCenterProj3d};
                    pp.swizzle(0,2);
                    pp*=globalScale;
                    return pp;
                };

                C4f color = C4fc::XTORGBA(group.colour);
                // && element.type == "road"
                if ( group.part.empty() ) {
                    for ( auto ti = 0u; ti < group.triangles.size(); ti++ ) {
                        auto v1 = mp(ti);
                        mesh.addVertex( v1, V4fc::ZERO, color*10.0 );
                    }
                } else if ( group.part == "roof_faces" ) {
                    std::vector<V2f> rootPoints{};
                    for ( auto ti = 0u; ti < group.triangles.size(); ti++ ) {
                        V3f p = mp(ti);
                        rootPoints.emplace_back( V2f(p.xz()) );
                    }
                    auto gObb = RotatingCalipers::minAreaRect(rootPoints);

                    float yOff = 12.0f + static_cast<float>(unitRandI(3)) ;
                    V2f tile{static_cast<float>(unitRandI(15))/16.0f, yOff/16.0f};
                    for ( auto ti = 0u; ti < group.triangles.size(); ti++ ) {
                        auto v1 = mp(ti);
                        V2f uv1 = dominantMapping( V3f::UP_AXIS(), v1, V3fc::ONE );
                        uv1*=V2fc::ONE*(1.0f/globalScale)*facadeMappingScale;
                        uv1.rotate(gObb.angle_width);
                        mesh.addVertex( v1, V4f{uv1, tile}, color );
                    }
                } else if ( group.part == "lateral_faces" ) {
                    auto yOff = static_cast<float>(unitRandI(12));
                    V2f tile{static_cast<float>(unitRandI(15))/16.0f, yOff/16.0f};
                    float xAcc = 0.0f;
                    for ( auto ti = 0u; ti < group.triangles.size(); ti+=6 ) {

                        auto v1 = mp(ti);
                        auto v2 = mp(ti+1);
                        auto v3 = mp(ti+2);
                        auto v4 = mp(ti+3);
                        auto v5 = mp(ti+4);
                        auto v6 = mp(ti+5);

                        float dist = distance(v1, v2);
                        V2f uv1 = V2f{xAcc, -v1.y()};
                        V2f uv2 = V2f{xAcc + dist, -v2.y()};
                        V2f uv3 = V2f{xAcc + dist, -v3.y()};
                        V2f uv4 = V2f{xAcc, -v4.y()};
                        V2f uv5 = V2f{xAcc + dist, -v5.y()};
                        V2f uv6 = V2f{xAcc, -v6.y()};

                        mesh.addVertex( v1, V4f{uv1*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        mesh.addVertex( v2, V4f{uv2*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        mesh.addVertex( v3, V4f{uv3*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        mesh.addVertex( v4, V4f{uv4*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        mesh.addVertex( v5, V4f{uv5*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        mesh.addVertex( v6, V4f{uv6*V2fc::ONE*(1.0f/globalScale)*facadeMappingScale, tile}, color );
                        xAcc += dist;
                    }
                }

                for ( auto ti = indexOffset; ti < indexOffset + group.triangles.size(); ti+=3 ) {
                    if ( !isCollinear(mesh.vertices[ti], mesh.vertices[ti+1], mesh.vertices[ti+2]) ) {
                        mesh.addTriangle(ti, ti+1, ti+2);
                    }
                }
                indexOffset += group.triangles.size();
            }
        }

        PolyStruct ps = createGeom( mesh, V3fc::ONE, GeomMapping{ GeomMappingT::PreBaked}, 0, ReverseFlag::False );
        _ret->fill( ps );
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
