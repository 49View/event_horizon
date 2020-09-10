//
//  follower.cpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#include "follower.hpp"
#include <core/util.h>
#include <core/math/quaternion.h>
#include <poly/vdata_assembler.h>
#include <poly/poly_services.hpp>
#include "poly_services.hpp"

FollowerPoly::FollowerPoly( const std::vector<Vector3f>& rp1, const std::vector<Vector3f>& rp2,
                            const std::array<size_t, 4>& indices, WindingOrderT _wo ) {

    vindices = indices;

    auto vsm11 = rp1[indices[0]];
    auto vsm12 = rp2[indices[0]];

    vs[0] = rp1[indices[1]];
    vs[1] = rp2[indices[1]];
    vs[2] = rp1[indices[2]];
    vs[3] = rp2[indices[2]];

    auto vsp11 = rp1[indices[3]];
//    auto vsp12 = rp2[indices[3]];

    auto vnm1 = normalize(crossProduct(vsm11, vs[0], vsm12));
    auto vnp1 = normalize(crossProduct(vs[2], vsp11, vs[3]));

    int i1 = _wo == WindingOrder::CCW ? 1 : 2;
    int i2 = _wo == WindingOrder::CCW ? 2 : 1;
    vn = normalize(crossProduct(vs[0], vs[i2], vs[i1]));

    float d1 = dot(vn, vnm1);
    float d2 = dot(vn, vnp1);

    Vector3f an1 = d1 > 0.01f ? normalize(vn + vnm1) : vn;
    Vector3f an2 = d2 > 0.01f ? normalize(vn + vnp1) : vn;
    vncs[0] = an1;
    vncs[2] = an2;
    vncs[1] = vncs[0];
    vncs[3] = vncs[2];
}

const FollowerGap FollowerGap::Empty(0);

bool FollowerGap::isGapAt( uint64_t index ) const {
    if ( index >= mGaps.size()) return false;
    return mGaps[index].side != FollowerGapSide::NoGap;
}

bool FollowerGap::isStartGapAt( uint64_t index ) const {
    if ( index >= mGaps.size()) return false;
    return mGaps[index].side == FollowerGapSide::Start;
}

bool FollowerGap::isEndGapAt( uint64_t index ) const {
    if ( index >= mGaps.size()) return false;
    return mGaps[index].side == FollowerGapSide::End;
}

bool FollowerGap::isVisibleAt( uint64_t index ) const {
    ASSERT(index < mGaps.size());
    return mGaps[index].side != FollowerGapSide::NotVisible && mGaps[index].side != FollowerGapSide::Start;
}

void FollowerGap::tagAllNotVisible() {
    for ( int64_t t = 0; t < static_cast<int64_t>( mGaps.size()); t++ ) {
        if ( mGaps[t].side == FollowerGapSide::Start ) {
            for ( int64_t m = 0; m < static_cast<int64_t>( mGaps.size()); m++ ) {
                int64_t index = getCircularArrayIndex(t + m + 1, static_cast<int64_t>( mGaps.size()));
                if ( mGaps[index].side == FollowerGapSide::End ) break;
                mGaps[index].side = FollowerGapSide::NotVisible;
            }
        }
    }
}

bool FollowerGap::isGap( FollowerGapSide side, uint64_t index, float& inset ) const {
    if ( index >= mGaps.size()) return false;
    if ( mGaps[index].side != side ) return false;

    inset = mGaps[index].inset;
    return true;
}

Vector3f suggestedFollowerAxisCalcBestAxis( const std::vector<Vector3f>& _verts ) {

    auto vaCount = _verts.size();
    if ( vaCount > 2 ) {
        return normalize(crossProduct(_verts[0], _verts[1], _verts[2]));
    } else {
        return normalize(crossProduct(_verts[0], _verts[1], _verts[1] + Vector3f{ unitRand() }));
    }

}

Vector3f suggestedFollowerAxis( const std::vector<Vector3f>& _verts, const Vector3f& _suggestedAxis ) {

    Vector3f ret = _suggestedAxis;
    if ( _suggestedAxis == V3fc::ZERO ) {
        ret = suggestedFollowerAxisCalcBestAxis(_verts);
    } else {
        // Check that the suggested axis is not bonkers
        Vector3f ncc = crossProduct(_verts[0], _verts[1], _verts[0] + ret);
        if ( !isValid(ncc.x()) || isScalarEqual(ncc.linearSum(), 0.0f)) {
            ret = suggestedFollowerAxisCalcBestAxis(_verts);
        }
    }
    return ret;
}

namespace FollowerService {

    std::vector<Vector3f> createLinePath( const V2f& a, const V2f& b, float width, float z ) {
        std::vector<Vector3f> fverts;
        auto pdir = normalize(b - a);
        auto cross = rotate90(pdir);
//        fverts.emplace_back(a + cross * width * 0.5f, z );
//        fverts.emplace_back(a - cross * width * 0.5f, z );
//        fverts.emplace_back(b - cross * width * 0.5f, z );
//        fverts.emplace_back(b + cross * width * 0.5f, z );

        fverts.emplace_back(b + cross * width * 0.5f, z );
        fverts.emplace_back(b - cross * width * 0.5f, z );
        fverts.emplace_back(a - cross * width * 0.5f, z );
        fverts.emplace_back(a + cross * width * 0.5f, z );

        return XZY::C(fverts);
    }

    std::vector<Vector3f> rotateAndIntersectData( const std::vector<Vector3f>& source, const Plane3f& lvplanes,
                                                  const Vector3f vn ) {
        // UVs, the "x" axis is running parallel to vcoords and it's just the length value
        // UVs, the "y" axis is running wrapping the profile, and it's just the length value
        // the "y"s have got 1 more entry in the array because they need to hold the total length to avoid wrapping around zero
        std::vector<Vector3f> ret;
        for ( auto m : source ) {
            Vector3f vce2 = m + ( vn * 1000000.0f );
            ret.push_back(lvplanes.intersectLineGrace(m, vce2));
        }
        return ret;
    }

    void addLineVert( const std::vector<Vector3f>& _verts, FollowerIntermediateData& _fid, size_t _m, bool _bWrap ) {

        bool isLastWrap = ( _bWrap && ( _m == _verts.size()));

        Vector3f vleft = getLeftVectorFromList(_verts, _m, _bWrap);
        Vector3f vright = getRightVectorFromList(_verts, _m, _bWrap);
        Vector3f vpos = _verts[_m];

        // This is to add and extra vert in case the path is wrapped
        if ( isLastWrap ) {
            vleft = _verts[_verts.size() - 1];
            vright = _verts[0];
            vpos = mix(vleft, vright, 0.5f);
            _fid.vcoords.push_back(_verts.front());
        } else {
            _fid.vcoords.push_back(vpos);
        }

        Vector3f vposPlanePerpL = crossProduct(vleft, vpos, vpos + _fid.vplanet);
        Vector3f vposPlanePerpR = crossProduct(vpos + _fid.vplanet, vpos, vright);

        Vector3f vn = normalize(vposPlanePerpL + vposPlanePerpR);
        Vector3f vd = normalize(vright - vpos);
        Vector3f vl = normalize(vpos - vleft);
        Vector3f vdn = normalize(vd + vl);

        _fid.vplanesb.push_back(vn);

        if ( isLastWrap ) {
            _fid.vdirs.push_back(normalize(_verts.front() - _verts.back()));
            _fid.vplanesn.emplace_back(_fid.vplanesn.front());
        } else {
            _fid.vplanesn.emplace_back(vdn, vpos);
            _fid.vdirs.push_back(vd);
        }

    }

    void extrude( std::shared_ptr<VData> geom,
                  const std::vector<Vector3f>& _verts,
                  const Profile& profile,
                  const Vector3f& _suggestedAxis,
                  const FollowerFlags& ff ) {

        GeomMappingData mapping;

        bool bWrap = checkBitWiseFlag(ff, FollowerFlags::WrapPath);

        // Sanitize same points and collinear
        std::vector<Vector3f> verts;
        verts = sanitizePath(_verts, bWrap);

        auto vaCount = verts.size();
        if ( vaCount < 2 ) {
            LOGR("[ERROR] Extruded geometry has less then 2 vertices, impossible to extrude.");
            return;
        }
        if ( vaCount == 2 ) bWrap = false;

        FollowerIntermediateData fid;

        fid.vplanet = suggestedFollowerAxis(verts, _suggestedAxis);

        for ( size_t m = 0; m < vaCount + bWrap; m++ ) {
            addLineVert(verts, fid, m, bWrap);
        }

        // Temp data for triangulation
        std::vector<Vector3f> rp2;
        std::vector<Vector3f> rp1;
        std::vector<FollowerPoly> polys;

        MappingServices::resetWrapMapping(mapping, profile.Lengths());
        auto wo = detectWindingOrder(profile.Points());
        mapping.windingOrder = wo;

        auto vcFinalSize = fid.vcoords.size();
        // Pre-loop setup, allocate/setup first element
        if ( bWrap ) {
            rp2 = profile.rotatePoints(fid.vplanesb[vcFinalSize - 1], fid.vplanet, fid.vcoords[vcFinalSize - 2]);
            rp1 = rotateAndIntersectData(rp2, fid.vplanesn[0], fid.vdirs[fid.vcoords.size() - 1]);
        } else {
            rp1 = profile.rotatePoints(fid.vplanesb[0], fid.vplanet, fid.vcoords[0]);
        }

        int wrapIndex = rp1.size() > 2 ? 0 : 1;

        // This triangulate the shape by wrapping points between 2 joints
        for ( size_t t = 0; t < vcFinalSize - 1; t++ ) {

            rp2 = rotateAndIntersectData(rp1, fid.vplanesn[t + 1], fid.vdirs[t]);

            for ( uint64_t m = 0; m < rp1.size() - wrapIndex; m++ ) {
                auto mi = static_cast<int64_t >(m);
                auto rpsizei = static_cast<int64_t >(rp1.size());
                auto mn1 = static_cast<uint64_t>(getCircularArrayIndex(mi - 1, rpsizei));
                auto nextIndex = static_cast<uint64_t>(getCircularArrayIndex(mi + 1, rpsizei));
                auto nextIndexp1 = static_cast<uint64_t>(getCircularArrayIndex(mi + 2, rpsizei));

                FollowerPoly fp{ rp1, rp2, { static_cast<size_t>(mn1), static_cast<size_t>(m),
                                             static_cast<size_t>(nextIndex), static_cast<size_t>(nextIndexp1) }, wo };

                if ( checkBitWiseFlag(ff, FollowerFlags::UsePlanarMapping)) {
                    MappingServices::planarMapping(mapping, absolute(fp.vn), fp.vs.data(), fp.vtcs.data(), 4);
                } else {
                    MappingServices::updateWrapMapping(mapping, fp.vs.data(), fp.vtcs.data(), m, rp1.size());
                }

                polys.push_back(fp);
            }

            // Make previous next array current array
            rp1 = rp2;
        }

        // Add all the polys
        for ( auto& fp : polys ) {
            PolyServices::addQuad(geom, fp.vs, fp.vtcs, fp.vncs, mapping);
        }
    }

}

Follower::Follower( subdivisionAccuray subDivs /*= accuracyNone*/,
                    const FollowerFlags& ff /*= FollowerFlags::Defaults*/,
                    const std::string& _name /*= "FollowerUnnamed"*/,
                    const MappingDirection _md /*= MappingDirection::X_POS*/, bool _bUseFlatMapping /*= false */ ) {
    CurrentFlags(ff);
    mSubDivAcc = subDivs;
    mGeomName = _name;
    mGeomType = 0;
    mCapsGeomType = 0;
    mbUsePlanarMapping = _bUseFlatMapping;
    mMappingDirection = _md;
}

Vector3f rotateFollowerPlane90( const Vector3f& vpos, const Vector3f& vn3d ) {
    Quaternion qp{ vpos, 0.0f };
    float halfAngle = M_PI_2;
    Quaternion r1{ halfAngle, vn3d };
    Quaternion r2{ -halfAngle, vn3d };
    Quaternion vqn = r1 * qp * r2;
    return vqn.vector();
}

void Follower::reserveVBounding( uint64_t size ) {
    // Reserve space for the vBoundingContours
    for ( uint64_t vbi = 0; vbi <= size * 2 - 1; vbi++ ) {
        vboundingContours.push_back(V3fc::ZERO);
    }
}

void
Follower::createQuadPath( std::vector<Vector2f>& fverts, float width, float height, PivotPointPosition /*alignment*/ ) {
    fverts.clear();
    fverts.emplace_back(Vector2f(-width * 0.5f, -height * 0.5f));
    fverts.emplace_back(Vector2f(-width * 0.5f, height * 0.5f));
    fverts.emplace_back(Vector2f(width * 0.5f, height * 0.5f));
    fverts.emplace_back(Vector2f(width * 0.5f, -height * 0.5f));
}

//bool Follower::skipGapAt( int t ) {
//    return ( mGaps.isStartGapAt( t + 1 ) && mGaps.isVisibleAt( t + 1 ));
//}

//void averagePoly( std::vector<FollowerPoly>& polys, uint64_t pi, uint64_t i,
//                  uint64_t i1, uint64_t i2, uint64_t i3 ) {
////    Vector3f vn = polys[pi].vn;
////    polys[pi].vncs[i] = vn;
//
////	float w1 = dot( vn,  polys[i1].vn ) > 0.05f ? 1.0f : 0.0f;
////	polys[pi].vncs[i] += polys[i1].vn * w1;
////
////	w1 = dot( vn, polys[i2].vn ) > 0.05f ? 1.0f : 0.0f;
////	polys[pi].vncs[i] += polys[i2].vn * w1;
////
////	w1 = dot( vn, polys[i3].vn ) > 0.05f ? 1.0f : 0.0f;
////	polys[pi].vncs[i] += polys[i3].vn * w1;
//
////    polys[pi].vncs[i] = normalize( polys[pi].vncs[i] );
//}

std::vector<Vector2f> Follower::vboundingContours2f() const {
    std::vector<Vector2f> ret;
    for ( auto v : vboundingContours ) {
        ret.push_back(v.xy());
    }
    return ret;
}

void Follower::type( NodeType _gt ) {
    mGeomType |= _gt;
    mCapsGeomType |= _gt;
}

void Follower::capsType( NodeType _gt ) {
    mCapsGeomType |= _gt;
}

//void Follower::excludeAxisFromExtrusion( const Vector3f& _axis ) {
//    mAxisExtrudeExclusions.push_back( _axis );
//}

std::string Follower::Name() const {
    return mGeomName;
}

void Follower::Name( const std::string& val ) {
    mGeomName = val;
}
