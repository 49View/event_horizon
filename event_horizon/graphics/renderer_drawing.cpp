//
// Created by Dado on 27/12/2017.
//

#include "renderer.h"
#include <core/TTF.h>
#include <core/raw_image.h>
#include <core/image_mapping.hpp>
#include <graphics/vp_builder.hpp>

static std::shared_ptr<HeterogeneousMap> mapColor( const Color4f& _matColor ) {
    auto values = std::make_shared<HeterogeneousMap>();
    values->assign( UniformNames::opacity, _matColor.w() );
    values->assign( UniformNames::diffuseColor, _matColor.xyz() );
    return values;
}

static std::shared_ptr<HeterogeneousMap> mapTextureAndColor( const std::string& _tname, const Color4f& _matColor ) {
    auto values = std::make_shared<HeterogeneousMap>();
    values->assign( UniformNames::colorTexture, _tname );
    values->assign( UniformNames::opacity, _matColor.w() );
    values->assign( UniformNames::diffuseColor, _matColor.xyz() );
    return values;
}

void Renderer::drawIncGridLines( const int bucketIndex, int numGridLines, float deltaInc, float gridLinesWidth,
                       const Vector3f& constAxis0, const Vector3f& constAxis1, const Color4f& smallAxisColor,
                       const float zoffset, const std::string& _name ) {
    float delta = deltaInc;
    for ( int t = 0; t < numGridLines; t++ ) {
        // xLines
        Vector3f lerpLeftX = Vector3f( constAxis0.dominantElement() == 0 ? constAxis0.x() : delta, zoffset,
                                       constAxis0.dominantElement() == 2 ? constAxis0.z() : delta );
        Vector3f lerpRightX = Vector3f( constAxis1.dominantElement() == 0 ? constAxis1.x() : delta, zoffset,
                                        constAxis1.dominantElement() == 2 ? constAxis1.z() : delta );
        drawLine( bucketIndex, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth, false, 0.0f, 1.0f,
                  _name + std::to_string( t ) + "+" );
        lerpLeftX = Vector3f( constAxis0.dominantElement() == 0 ? constAxis0.x() : -delta, zoffset,
                              constAxis0.dominantElement() == 2 ? constAxis0.z() : -delta );
        lerpRightX = Vector3f( constAxis1.dominantElement() == 0 ? constAxis1.x() : -delta, zoffset,
                               constAxis1.dominantElement() == 2 ? constAxis1.z() : -delta );
        drawLine( bucketIndex, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth, false, 0.0f, 1.0f,
                  _name + std::to_string( t ) + "-" );
        delta += deltaInc;
    }
}

void Renderer::createGrid( const int bucketIndex, float unit, const Color4f& mainAxisColor,
                           const Color4f& smallAxisColor, const Vector2f& limits, const float axisSize,
                           const std::string& _name ) {
    float mainAxisWidth = axisSize;
    float gridLinesWidth = mainAxisWidth * 0.5f;
    float zoffset = 0.0f;

    Vector3f leftXAxis  = Vector3f( -limits.x(), zoffset, 0.0f );
    Vector3f rightXAxis = Vector3f(  limits.x(), zoffset, 0.0f );

    Vector3f topYAxis =    Vector3f( 0.0f, zoffset, -limits.y() );
    Vector3f bottomYAxis = Vector3f( 0.0f, zoffset,  limits.y() );

    Vector2f axisLenghts = { limits.x() * 2.0f, limits.y() * 2.0f };

    drawIncGridLines( bucketIndex, static_cast<int>(( axisLenghts.y() / unit ) / 2.0f ), unit, gridLinesWidth, leftXAxis,
                      rightXAxis, smallAxisColor, zoffset-0.01f, _name + "y_axis" );
    drawIncGridLines( bucketIndex, static_cast<int>(( axisLenghts.x() / unit ) / 2.0f ), unit, gridLinesWidth, topYAxis,
                      bottomYAxis, smallAxisColor, zoffset-0.01f, _name + "x_axis" );

    // Main axis
    drawLine( bucketIndex, leftXAxis, rightXAxis, mainAxisColor, mainAxisWidth, false, 0.0f, 1.0f, _name + "xAxis" );
    drawLine( bucketIndex, topYAxis, bottomYAxis, mainAxisColor, mainAxisWidth, false, 0.0f, 1.0f, _name + "yAxis" );
}

void Renderer::createGridV2( const int bucketIndex, float unit, const Color4f& mainAxisColor,
                           const Color4f& smallAxisColor, const Vector2f& limits, const float axisSize,
                           const std::string& _name ) {
    float mainAxisWidth = axisSize;
    float gridLinesWidth = mainAxisWidth * 0.5f;

    Vector3f leftXAxis  = V3f{ -limits.x(), 0.0f, 0.0f };
    Vector3f rightXAxis = V3f{  limits.x(), 0.0f, 0.0f };

    Vector3f topYAxis =    Vector3f( 0.0f, 0.0f, -limits.y() );
    Vector3f bottomYAxis = Vector3f( 0.0f, 0.0f,  limits.y() );

    Vector2f axisLenghts = { limits.x() * 2.0f, limits.y() * 2.0f };

    auto numGridLinesY = static_cast<int>(( axisLenghts.y() / unit ));
    float delta = topYAxis.z();
    for ( int t = 0; t < numGridLinesY; t++ ) {
        // xLines
        Vector3f lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
        Vector3f lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
//        drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
        for ( int q = 0; q < 3; q++ ) {
            delta += unit * 0.25f;
            lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
            lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
            drawLine( bucketIndex, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth*0.75f );
        }
        delta += unit * 0.25f;
//        if ( t == numGridLinesY - 1 ) {
//            lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
//            lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
//            drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth);
//        }
    }

    auto numGridLinesX = static_cast<int>(( axisLenghts.x() / unit ) );
    delta = leftXAxis.x();
    for ( int t = 0; t < numGridLinesX; t++ ) {
        // xLines
        Vector3f lerpLeftX = topYAxis + V3f::X_AXIS * delta;
        Vector3f lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
//        drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
        for ( int q = 0; q < 3; q++ ) {
            delta += unit * 0.25f;
            lerpLeftX = topYAxis + V3f::X_AXIS * delta;
            lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
            drawLine( bucketIndex, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth*0.75f );
        }
        delta += unit * 0.25f;
//        if ( t == numGridLinesX - 1 ) {
//            lerpLeftX = topYAxis + V3f::X_AXIS * delta;
//            lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
//            drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
//        }
    }





    delta = topYAxis.z();
    for ( int t = 0; t < numGridLinesY; t++ ) {
        // xLines
        Vector3f lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
        Vector3f lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
        drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
        for ( int q = 0; q < 3; q++ ) {
            delta += unit * 0.25f;
            lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
            lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
        }
        delta += unit * 0.25f;
        if ( t == numGridLinesY - 1 ) {
            lerpLeftX = leftXAxis + V3f::Z_AXIS * delta;
            lerpRightX = rightXAxis + V3f::Z_AXIS * delta;
            drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth);
        }
    }

    delta = leftXAxis.x();
    for ( int t = 0; t < numGridLinesX; t++ ) {
        // xLines
        Vector3f lerpLeftX = topYAxis + V3f::X_AXIS * delta;
        Vector3f lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
        drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
        for ( int q = 0; q < 3; q++ ) {
            delta += unit * 0.25f;
            lerpLeftX = topYAxis + V3f::X_AXIS * delta;
            lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
//            drawLine( bucketIndex, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth*0.75f );
        }
        delta += unit * 0.25f;
        if ( t == numGridLinesX - 1 ) {
            lerpLeftX = topYAxis + V3f::X_AXIS * delta;
            lerpRightX = bottomYAxis + V3f::X_AXIS * delta;
            drawLine( bucketIndex, lerpLeftX, lerpRightX, mainAxisColor, gridLinesWidth );
        }
    }
}

VPListSP Renderer::drawRect( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, CResourceRef _texture,
                         float ratio, const Color4f& color, RectFillMode fm, const std::string& _name ) {
    Rect2f rect{ p1, p2, true };
    QuadVertices2 qvt = textureQuadFillModeMapping( fm, rect, ratio );
    auto ps = std::make_shared<PosTex3dStrip>( rect, qvt, 0.0f );
    auto vp = VPBuilder<PosTex3dStrip>{*this,ShaderMaterial{S::TEXTURE_3D, mapTextureAndColor(_texture, color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawRect( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Color4f& color,
                           const std::string& _name ) {

    auto ps = std::make_shared<Pos3dStrip>( Rect2f{ p1, p2, true }, 0.0f );
    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawRect2d( const int bucketIndex, const Rect2f& r1, const Color4f& color, const std::string& _name ) {
    auto ps = std::make_shared<Pos3dStrip>( r1 );
    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_2D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawRect2d( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Color4f& color,
                           const std::string& _name ) {

    return drawRect2d( bucketIndex, Rect2f{ p1, p2, true }, color, _name );
}

VPListSP Renderer::drawRect2d( const int bucketIndex, const Rect2f& rect, CResourceRef _texture,
                           float ratio, const Color4f& color, RectFillMode fm, const std::string& _name ) {
//    QuadVertices2 qvt = textureQuadFillModeMapping( fm, rect, ratio );
//    auto ps = std::make_shared<PosTex3dStrip>( rect, qvt );
    auto ps = std::make_shared<PosTex3dStrip>( rect, QuadVertices2::QUAD_TEX_STRIP_INV_Y_COORDS );
    auto vp = VPBuilder<PosTex3dStrip>{*this,ShaderMaterial{S::TEXTURE_2D, mapTextureAndColor(_texture, color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawRect2d( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, CResourceRef _texture,
                         float ratio, const Color4f& color, RectFillMode fm, const std::string& _name ) {
    Rect2f rect{ p1, p2 };
    return drawRect2d( bucketIndex, rect, _texture, ratio, color, fm, _name );
}

VPListSP Renderer::drawArrow( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Vector4f& color,
                          float width, float angle, float arrowlength, float _z, float percToBeDrawn,
                          const std::string& _name1, const std::string& _name2 ) {
    std::vector<Vector3f> vlist;
    Vector2f n = normalize( p2 - p1 );
    auto pn1 = rotate( n, angle );
    auto pn2 = rotate( n, -angle );
    vlist.emplace_back( p1 + pn1 * arrowlength, _z );
    vlist.emplace_back( p1, _z );
    vlist.emplace_back( p1 + pn2 * arrowlength, _z );
    drawLine( bucketIndex, vlist, color, width, false, 0.0f, percToBeDrawn, _name1 );
    // ### FIXME VP
    return drawLine( bucketIndex, p1, p2, color, width * 0.75f, false, 0.0f, percToBeDrawn, _name2 );
}

VPListSP Renderer::drawLine( int bucketIndex, const Vector3f& p1, const Vector3f& p2, const Vector4f& color, float width,
               bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> vlist;

    vlist.push_back( p1 );
    vlist.push_back( p2 );

    return drawLine( bucketIndex, vlist, color, width, wrapIt, rotAngle, percToBeDrawn, _name );
}

VPListSP Renderer::drawLine( int bucketIndex, const std::vector<Vector2f>& verts, float z, const Vector4f& color, float width,
          bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> vlist;
    for ( auto& v : verts ) vlist.push_back( { v, z } );
    return drawLine( bucketIndex, vlist, color, width, wrapIt, rotAngle, percToBeDrawn, _name );
}

VPListSP Renderer::drawLine( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color, float width,
               bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    if ( verts.size() < 2 ) return nullptr;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>();

    colorStrip->generateStripsFromVerts( extrudePointsWithWidth<ExtrudeStrip>( verts, width, wrapIt ), wrapIt );

    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawTriangle( int bucketIndex, const std::vector<Vector2f>& verts, float _z, const Vector4f& color,
                   const std::string& _name ) {
    if ( verts.size() != 3 ) return nullptr;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( 3, 3, PRIMITIVE_TRIANGLE_STRIP,
                                                                           VFVertexAllocation::PreAllocate );
    colorStrip->addStripVertex( Vector3f{ verts[0], _z } );
    colorStrip->addStripVertex( Vector3f{ verts[1], _z } );
    colorStrip->addStripVertex( Vector3f{ verts[2], _z } );
    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawTriangle( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
                   const std::string& _name ) {
    if ( verts.size() != 3 ) return nullptr;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( 3, 3, PRIMITIVE_TRIANGLE_STRIP,
                                                                           VFVertexAllocation::PreAllocate );
    colorStrip->addStripVertex( verts[0] );
    colorStrip->addStripVertex( verts[1] );
    colorStrip->addStripVertex( verts[2] );
    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color
        ,	const std::string& _name) {
    //Multiple of 3
    if (verts.size()==0 || verts.size()%3 != 0) return nullptr;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( static_cast<int32_t>(verts.size()),
                                                                           static_cast<int32_t>(verts.size()),
                                                                           PRIMITIVE_TRIANGLES,
                                                                           VFVertexAllocation::PreAllocate );
    for (auto &v : verts) {
        colorStrip->addStripVertex(v);
    }
    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const std::vector<int32_t>& indices,
                   const Vector4f& color, const std::string& _name) {
    //Multiple of 3.d
    if (verts.size()==0 || indices.size()==0 || indices.size() % 3 != 0) return nullptr;

    std::unique_ptr<uint32_t[]> i = std::make_unique<uint32_t[]>(indices.size());

    memcpy(i.get(), indices.data(), sizeof(int32_t)*indices.size());

    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( static_cast<int32_t>(verts.size()),
                                                                           PRIMITIVE_TRIANGLES,
                                                                           VFVertexAllocation::PreAllocate,
                                                                           static_cast<uint32_t>(indices.size()),
                                                                           i );

    for (auto& v : verts) {
        colorStrip->addVertex(v);
    }

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawCylinder( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f&
color, float size,
              const std::string& _name ) {
    drawLine( bucketIndex, pos, dir, color, size, false, 0.0f, 1.0f, _name );
    //### FIXME VP
    return drawLine( bucketIndex, pos, dir, color, size, false, M_PI_2, 1.0f, _name );
}

VPListSP Renderer::drawCone( int bucketIndex, const Vector3f& /*posBase*/, const Vector3f& /*posTop*/,
               const Vector4f& /*color*/, float /*size*/, const std::string& /*_name*/ ) {
    return nullptr;
}

VPListSP Renderer::draw3dVector( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
              const std::string& _name ) {
//    draw3dPoint( bucketIndex, pos, color, size * 1.25f, _name );
    return drawCylinder( bucketIndex, pos, pos + dir, color, size, _name );
}

VPListSP Renderer::drawArc( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
              const Vector4f& color, float width, int32_t subdivs, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> points;
    for ( int t = 0; t < subdivs; t++ ) {
        float delta = ( static_cast<float>( t ) / static_cast<float>( subdivs - 1 ));
        float angle = JMATH::lerp( delta, fromAngle, toAngle );
        points.push_back( Vector3f( center.xy() + Vector2f( sinf( angle ), cosf( angle )) * radius, center.z()));
    }

    return drawLine( bucketIndex, points, color, width, false, 0.0f, percToBeDrawn, _name );
}

VPListSP Renderer::drawArc( int bucketIndex, const Vector3f& center, const Vector3f& p1, const Vector3f& p2,
              const Vector4f& color, float width, int32_t subdivs, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> points;

    Vector2f n1 = normalize(( p1 - center ).xy());
    Vector2f n2 = normalize(( p2 - center ).xy());

    float radius = length( center - p2 );
    for ( int t = 0; t < subdivs; t++ ) {
        float delta = ( static_cast<float>( t ) / static_cast<float>( subdivs - 1 ));
        Vector3f pm = center + Vector3f( normalize( lerp( delta, n1, n2 )) * radius, center.z());
        points.push_back( pm );
    }

    return drawLine( bucketIndex, points, color, width, false, 0.0f, percToBeDrawn, _name );
}

VPListSP Renderer::drawArcFilled( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
                    const Vector4f& color, float /*width*/, int32_t subdivs, const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return nullptr;
    std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
    std::unique_ptr<VFPos3d[]> _verts = std::unique_ptr<VFPos3d[]>( new VFPos3d[numIndices] );

    for ( int t = 0; t < numIndices; t++ ) {
        _indices[t] = t;
    }
    _verts[0].pos = center;
    for ( int t = 1; t < numIndices; t++ ) {
        float delta = ( static_cast<float>( t - 1 ) / static_cast<float>( subdivs - 1 ));
        float angle = JMATH::lerp( delta, fromAngle, toAngle );
        _verts[t].pos = Vector3f( center.xy() + Vector2f( sinf( angle ), cosf( angle )) * radius, center.z());
    }

    std::shared_ptr<Pos3dStrip> ps = std::make_shared<Pos3dStrip>( numIndices, PRIMITIVE_TRIANGLE_FAN, numIndices,
                                                                   _verts, _indices );

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawDot( int bucketIndex, const Vector3f& center, float radius, const Color4f& color, const std::string& _name ) {
    auto numIndices = 5;
    std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
    std::unique_ptr<VFPos3d[]> _verts = std::unique_ptr<VFPos3d[]>( new VFPos3d[numIndices] );

    for ( int t = 0; t < numIndices; t++ ) {
        _indices[t] = t;
    }
    for ( int t = 0; t < numIndices; t++ ) {
        float angle = M_PI_4 + ( static_cast<float>( t - 1 ) / static_cast<float>( numIndices - 1 )) * TWO_PI;
        _verts[t].pos = Vector3f( center + V3f( sinf( angle ), 0.0f, cosf( angle )) * radius * sqrt(2.0f));
    }

    std::shared_ptr<Pos3dStrip> ps = std::make_shared<Pos3dStrip>( 4, PRIMITIVE_TRIANGLE_FAN, 4,
                                                                   _verts, _indices );

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawCircle( int bucketIndex, const Vector3f& center, float radius, const Color4f& color, int32_t subdivs,
            const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return nullptr;
    std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
    std::unique_ptr<VFPos3d[]> _verts = std::unique_ptr<VFPos3d[]>( new VFPos3d[numIndices] );

    for ( int t = 0; t < numIndices; t++ ) {
        _indices[t] = t;
    }
    _verts[0].pos = center;
    for ( int t = 1; t < numIndices; t++ ) {
        float angle = ( static_cast<float>( t - 1 ) / static_cast<float>( subdivs - 1 )) * TWO_PI;
        _verts[t].pos = Vector3f( center + V3f( sinf( angle ), 0.0f, cosf( angle )) * radius);
    }

    std::shared_ptr<Pos3dStrip> ps = std::make_shared<Pos3dStrip>( numIndices, PRIMITIVE_TRIANGLE_FAN, numIndices,
                                                                   _verts, _indices );

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawCircle2d( int bucketIndex, const Vector2f& center, float radius, const Color4f& color, int32_t subdivs,
                             const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return nullptr;
    std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
    std::unique_ptr<VFPos2d[]> _verts = std::unique_ptr<VFPos2d[]>( new VFPos2d[numIndices] );

    for ( int t = 0; t < numIndices; t++ ) {
        _indices[t] = t;
    }
    _verts[0].pos = center;
    for ( int t = 1; t < numIndices; t++ ) {
        float angle = ( static_cast<float>( t - 1 ) / static_cast<float>( subdivs - 1 )) * TWO_PI;
        _verts[t].pos = Vector2f(
                center + Vector2f( sinf( angle ), cosf( angle )) * ( getScreenAspectRatioVectorY * radius ));
    }

    std::shared_ptr<Pos2dStrip> ps = std::make_shared<Pos2dStrip>( numIndices, PRIMITIVE_TRIANGLE_FAN, numIndices,
                                                                   _verts, _indices );

    auto vp = VPBuilder<Pos2dStrip>{*this,ShaderMaterial{S::COLOR_2D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

VPListSP Renderer::drawCircle( int bucketIndex, const Vector3f& center, const Vector3f& normal, float radius,
                 const Color4f& color, int32_t subdivs, const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return nullptr;
    std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[numIndices] );
    std::unique_ptr<VFPos3d[]> _verts = std::unique_ptr<VFPos3d[]>( new VFPos3d[numIndices] );

    for ( int t = 0; t < numIndices; t++ ) {
        _indices[t] = t;
    }
    _verts[0].pos = center;
    Matrix4f rotMat = Matrix4f::IDENTITY;
    rotMat.setFromRotationAnglePos( normal, Vector3f::Z_AXIS, center );

    for ( int t = 1; t < numIndices; t++ ) {
        float angle = ( static_cast<float>( t - 1 ) / static_cast<float>( subdivs - 1 )) * TWO_PI;
        _verts[t].pos = rotMat.transform( Vector3f( Vector2f( sinf( angle ), cosf( angle )) * radius, 0.0f ));
    }

    std::shared_ptr<Pos3dStrip> ps = std::make_shared<Pos3dStrip>( numIndices, PRIMITIVE_TRIANGLE_FAN, numIndices,
                                                                   _verts, _indices );

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(ps).n(_name).build();
    VPL( bucketIndex, vp );
    return vp;
}

void Renderer::drawText( int bucketIndex, const std::string& text, const V3f& pos, float scale,
                         std::shared_ptr<Font> font, const Color4f& color ) {
    Vector2f cursor = Vector2f::ZERO;
    for ( char i : text ) {
        Utility::TTF::CodePoint cp( static_cast<Utility::TTFCore::ulong>(i));
        const Utility::TTF::Mesh& m = font->GetTriangulation( cp );

        // Don't draw an empty space
        if ( !m.verts.empty() ) {
            auto ps = std::make_shared<FontStrip>( m.verts.size(), PRIMITIVE_TRIANGLES, VFVertexAllocation::PreAllocate );

            for ( size_t t = 0; t < m.verts.size(); t+=3 ) {
                Vector2f p1{ m.verts[t].pos.x  , ( m.verts[t].pos.y ) };
                Vector2f p3{ m.verts[t+1].pos.x, ( m.verts[t+1].pos.y ) };
                Vector2f p2{ m.verts[t+2].pos.x, ( m.verts[t+2].pos.y ) };
                ps->addVertex( XZY::C( p1 + cursor ), m.verts[t].texCoord, m.verts[t].coef );
                ps->addVertex( XZY::C( p2 + cursor ), m.verts[t+2].texCoord, m.verts[t+2].coef );
                ps->addVertex( XZY::C( p3 + cursor ), m.verts[t+1].texCoord, m.verts[t+1].coef );
            }

            auto trams = std::make_shared<Matrix4f>(pos, Vector3f::ZERO, V3f{scale * Font::gliphScaler()} * V3f{1.0f, -1.0f, -1.0f} );
            auto vp = VPBuilder<FontStrip>{*this,ShaderMaterial{S::FONT, mapColor(color)}}.p(ps).t(trams).n(text).build();
            VPL( bucketIndex, vp );
        }

        Utility::TTFCore::vec2f kerning = font->GetKerning( Utility::TTF::CodePoint( i ), cp );
        Vector2f nextCharPos = V2f( kerning.x, kerning.y );
        cursor += nextCharPos;
    }
}
