//
// Created by Dado on 27/12/2017.
//

#include "renderer.h"
#include <core/TTF.h>
#include <graphics/vp_builder.hpp>

static std::shared_ptr<HeterogeneousMap> mapColor( const Color4f& _matColor ) {
    auto values = std::make_shared<HeterogeneousMap>();
    values->assign( UniformNames::opacity, _matColor.w() );
    values->assign( UniformNames::diffuseColor, _matColor.xyz() );
    return values;
}

void Renderer::drawIncGridLines( int numGridLines, float deltaInc, float gridLinesWidth,
                       const Vector3f& constAxis0, const Vector3f& constAxis1, const Color4f& smallAxisColor,
                       const float zoffset, const std::string& _name ) {
    float delta = deltaInc;
    for ( int t = 0; t < numGridLines; t++ ) {
        // xLines
        Vector3f lerpLeftX = Vector3f( constAxis0.dominantElement() == 0 ? constAxis0.x() : delta, zoffset,
                                       constAxis0.dominantElement() == 2 ? constAxis0.z() : delta );
        Vector3f lerpRightX = Vector3f( constAxis1.dominantElement() == 0 ? constAxis1.x() : delta, zoffset,
                                        constAxis1.dominantElement() == 2 ? constAxis1.z() : delta );
        drawLine( CommandBufferLimits::CoreGrid, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth, false, 0.0f, 1.0f,
                  _name + std::to_string( t ) + "+" );
        lerpLeftX = Vector3f( constAxis0.dominantElement() == 0 ? constAxis0.x() : -delta, zoffset,
                              constAxis0.dominantElement() == 2 ? constAxis0.z() : -delta );
        lerpRightX = Vector3f( constAxis1.dominantElement() == 0 ? constAxis1.x() : -delta, zoffset,
                               constAxis1.dominantElement() == 2 ? constAxis1.z() : -delta );
        drawLine( CommandBufferLimits::CoreGrid, lerpLeftX, lerpRightX, smallAxisColor, gridLinesWidth, false, 0.0f, 1.0f,
                  _name + std::to_string( t ) + "-" );
        delta += deltaInc;
    }
}

void Renderer::createGrid( float unit, const Color4f& /*mainAxisColor*/,
                           const Color4f& smallAxisColor, const Vector2f& limits, const float axisSize,
                           const float zoffset, bool _monochrome, const std::string& _name ) {
    float mainAxisWidth = axisSize * 0.25f;
    float gridLinesWidth = mainAxisWidth;

    Vector3f leftXAxis  = Vector3f( -limits.x(), zoffset, 0.0f );
    Vector3f rightXAxis = Vector3f(  limits.x(), zoffset, 0.0f );

    Vector3f topYAxis =    Vector3f( 0.0f, zoffset + 0.01f, -limits.y() );
    Vector3f bottomYAxis = Vector3f( 0.0f, zoffset + 0.01f,  limits.y() );

    Vector2f axisLenghts = { limits.x() * 2.0f, limits.y() * 2.0f };

    drawIncGridLines( static_cast<int>(( axisLenghts.y() / unit ) / 2.0f ), unit, gridLinesWidth, leftXAxis,
                      rightXAxis, smallAxisColor, zoffset-0.01f, _name + "y_axis" );
    drawIncGridLines( static_cast<int>(( axisLenghts.x() / unit ) / 2.0f ), unit, gridLinesWidth, topYAxis,
                      bottomYAxis, smallAxisColor, zoffset-0.01f, _name + "x_axis" );

    // Main axis
    Color4f mc = Color4f{ smallAxisColor * 1.5f }.A(1.0f);
    Color4f xAxisColorPos = _monochrome ? mc : Color4f::PASTEL_RED * 0.75f;
    Color4f xAxisColorNeg = _monochrome ? mc : Color4f::PASTEL_RED * 1.25f;
    Color4f yAxisColorPos = _monochrome ? mc : Color4f::PASTEL_CYAN * 0.75f;
    Color4f yAxisColorNeg = _monochrome ? mc : Color4f::PASTEL_CYAN * 1.25f;

    Vector3f base = { 0.0f, zoffset, 0.0f  };
    drawLine( CommandBufferLimits::CoreGrid, leftXAxis, base, xAxisColorPos, mainAxisWidth, false, 0.0f, 1.0f, _name + "xNeg" );
    drawLine( CommandBufferLimits::CoreGrid, base, rightXAxis, xAxisColorNeg, mainAxisWidth, false, 0.0f, 1.0f, _name + "xPos" );
    drawLine( CommandBufferLimits::CoreGrid, topYAxis, base, yAxisColorPos, mainAxisWidth, false, 0.0f, 1.0f, _name + "yNeg" );
    drawLine( CommandBufferLimits::CoreGrid, base, bottomYAxis, yAxisColorNeg, mainAxisWidth, false, 0.0f, 1.0f, _name + "yPos" );
}

void Renderer::drawArrow( const int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Vector4f& color,
                          float width, float angle, float arrowlength, float _z, float percToBeDrawn,
                          const std::string& _name1, const std::string& _name2 ) {
    std::vector<Vector3f> vlist;
    Vector2f n = normalize( p2 - p1 );
    auto pn1 = rotate( n, angle );
    auto pn2 = rotate( n, -angle );
    vlist.push_back( { p1 + pn1 * arrowlength, _z } );
    vlist.push_back( { p1, _z } );
    vlist.push_back( { p1 + pn2 * arrowlength, _z } );
    drawLine( bucketIndex, vlist, color, width, false, 0.0f, percToBeDrawn, _name1 );
    drawLine( bucketIndex, p1, p2, color, width * 0.75f, false, 0.0f, percToBeDrawn, _name2 );
}

void Renderer::drawLine( int bucketIndex, const Vector3f& p1, const Vector3f& p2, const Vector4f& color, float width,
               bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> vlist;

    vlist.push_back( p1 );
    vlist.push_back( p2 );

    drawLine( bucketIndex, vlist, color, width, wrapIt, rotAngle, percToBeDrawn, _name );
}

void Renderer::drawLine( int bucketIndex, const std::vector<Vector2f>& verts, float z, const Vector4f& color, float width,
          bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> vlist;
    for ( auto& v : verts ) vlist.push_back( { v, z } );
    drawLine( bucketIndex, vlist, color, width, wrapIt, rotAngle, percToBeDrawn, _name );
}

void Renderer::drawLine( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color, float width,
               bool wrapIt, float rotAngle, float percToBeDrawn, const std::string& _name ) {
    if ( verts.size() < 2 ) return;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>();

    colorStrip->generateStripsFromVerts( extrudePointsWithWidth( verts, width, wrapIt, rotAngle, percToBeDrawn ), wrapIt );

    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
}

void Renderer::drawTriangle( int bucketIndex, const std::vector<Vector2f>& verts, float _z, const Vector4f& color,
                   const std::string& _name ) {
    if ( verts.size() != 3 ) return;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( 3, 3, PRIMITIVE_TRIANGLE_STRIP,
                                                                           VFVertexAllocation::PreAllocate );
    colorStrip->addStripVertex( Vector3f{ verts[0], _z } );
    colorStrip->addStripVertex( Vector3f{ verts[1], _z } );
    colorStrip->addStripVertex( Vector3f{ verts[2], _z } );
    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
}

void Renderer::drawTriangle( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
                   const std::string& _name ) {
    if ( verts.size() != 3 ) return;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( 3, 3, PRIMITIVE_TRIANGLE_STRIP,
                                                                           VFVertexAllocation::PreAllocate );
    colorStrip->addStripVertex( verts[0] );
    colorStrip->addStripVertex( verts[1] );
    colorStrip->addStripVertex( verts[2] );
    auto vp = VPBuilder<Pos3dStrip>{*this, ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
}

void Renderer::drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color
        ,	const std::string& _name) {
    //Multiple of 3
    if (verts.size()==0 || verts.size()%3 != 0) return;
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( static_cast<int32_t>(verts.size()),
                                                                           static_cast<int32_t>(verts.size()),
                                                                           PRIMITIVE_TRIANGLES,
                                                                           VFVertexAllocation::PreAllocate );
    for (auto &v : verts) {
        colorStrip->addStripVertex(v);
    }
    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
}

void Renderer::drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const std::vector<int32_t>& indices,
                   const Vector4f& color, const std::string& _name) {
    //Multiple of 3.d
    if (verts.size()==0 || indices.size()==0 || indices.size() % 3 != 0) return;

    std::unique_ptr<int32_t[]> i = std::make_unique<int32_t[]>(indices.size());

    memcpy(i.get(), indices.data(), sizeof(int32_t)*indices.size());

    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( static_cast<int32_t>(verts.size()),
                                                                           PRIMITIVE_TRIANGLES,
                                                                           VFVertexAllocation::PreAllocate,
                                                                           static_cast<int32_t>(indices.size()),
                                                                           i );

    for (auto& v : verts) {
        colorStrip->addVertex(v);
    }

    auto vp = VPBuilder<Pos3dStrip>{*this,ShaderMaterial{S::COLOR_3D, mapColor(color)}}.p(colorStrip).n(_name).build();
    VPL( bucketIndex, vp );
}

void Renderer::drawCylinder( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f&
color, float size,
              const std::string& _name ) {
    drawLine( bucketIndex, pos, dir, color, size, false, 0.0f, 1.0f, _name );
    drawLine( bucketIndex, pos, dir, color, size, false, M_PI_2, 1.0f, _name );
}

void Renderer::drawCone( int bucketIndex, const Vector3f& /*posBase*/, const Vector3f& /*posTop*/,
               const Vector4f& /*color*/, float /*size*/, const std::string& /*_name*/ ) {
}

void Renderer::draw3dVector( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
              const std::string& _name ) {
//    draw3dPoint( bucketIndex, pos, color, size * 1.25f, _name );
    drawCylinder( bucketIndex, pos, pos + dir, color, size, _name );
}

void Renderer::drawArc( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
              const Vector4f& color, float width, int32_t subdivs, float percToBeDrawn, const std::string& _name ) {
    std::vector<Vector3f> points;
    for ( int t = 0; t < subdivs; t++ ) {
        float delta = ( static_cast<float>( t ) / static_cast<float>( subdivs - 1 ));
        float angle = JMATH::lerp( delta, fromAngle, toAngle );
        points.push_back( Vector3f( center.xy() + Vector2f( sinf( angle ), cosf( angle )) * radius, center.z()));
    }

    drawLine( bucketIndex, points, color, width, false, 0.0f, percToBeDrawn, _name );
}

void Renderer::drawArc( int bucketIndex, const Vector3f& center, const Vector3f& p1, const Vector3f& p2,
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

    drawLine( bucketIndex, points, color, width, false, 0.0f, percToBeDrawn, _name );
}

void Renderer::drawArcFilled( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
                    const Vector4f& color, float /*width*/, int32_t subdivs, const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return;
    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[numIndices] );
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
}

void Renderer::drawCircle( int bucketIndex, const Vector3f& center, float radius, const Color4f& color, int32_t subdivs,
            const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return;
    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[numIndices] );
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
}

void Renderer::drawCircle2d( int bucketIndex, const Vector2f& center, float radius, const Color4f& color, int32_t subdivs,
                             const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return;
    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[numIndices] );
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
}

void Renderer::drawCircle( int bucketIndex, const Vector3f& center, const Vector3f& normal, float radius,
                 const Color4f& color, int32_t subdivs, const std::string& _name ) {
    int32_t numIndices = subdivs + 1;
    if ( numIndices < 3 ) return;
    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[numIndices] );
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
}

void Renderer::drawText( int bucketIndex, const std::string& text, const V3f& pos, float scale,
                         std::shared_ptr<Font> font, const Color4f& color ) {
    //        VPBuilder<FontStrip>{*this}.vl(_vpl).p(fs).m(_vpl->Is2d() ? S::FONT_2D: S::FONT).c(color).n(_vpname).build();

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

//    fs->addVertex( XZY::C(pos4.xyz() + orig), Vector2f{ m.verts[t].texCoord, m.verts[t].coef } );

}
