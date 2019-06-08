//
// Created by Dado on 06/01/2018.
//

#include "ui_shape_builder.h"
#include <core/node.hpp>
#include <core/app_globals.h>
#include <core/TTF.h>
#include <core/image_mapping.hpp>
#include <core/font_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/geom.hpp>
#include <core/names.hpp>

uint64_t UIShapeBuilder::sid = 0;

UIShapeBuilder::UIShapeBuilder( SceneGraph& _sg, UIShapeType shapeType ) : shapeType( shapeType ), sg(_sg) {

    init();
}

UIShapeBuilder::UIShapeBuilder( SceneGraph& _sg, UIShapeType _shapeType, const std::string& _ti, float _fh ) : shapeType( _shapeType ), sg(_sg) {

    init();
    if ( _fh != 0.0f ) fh(_fh);
    ti(_ti);
}

//void createRoundedRect( std::shared_ptr<VPList> _vpl, const Vector2f& size, const Vector3f& _originalPos,
//                        const Color4f& color, std::shared_ptr<Texture> tex = nullptr,
//                        int effectsMask = int( UIRenderFlags::NoEffects ), float bevelPerc = 0.05f,
//                        RectCreateAnchor anchor = RectCreateAnchor::Bottom, RectFillMode fm = RectFillMode::AspectFill,
//                        const std::string& _name = "" );

//Rect2f createText3dWithRect( std::shared_ptr<VPList> _vpl, const std::string& msg, const Vector3f& startPos,
//                             const Vector2f& size, const Utility::TTFCore::Font& f, float height, const Vector4f& color,
//                             const Vector4f& bgcolor, const UiAlignElement& padding = UiAlignElement::ZERO,
//                             float bevel = 0.05f, RectCreateAnchor anchor = RectCreateAnchor::Bottom,
//                             const std::string& _vpname = "", bool centered = true, bool expand = false );

//Rect2f Renderer::createText3dWithRect( std::shared_ptr<VPList> _vpl, const std::string& msg, const Vector3f& startPos,
//                                       const Vector2f& size, const Utility::TTFCore::Font& f, float height, const Vector4f& color,
//                                       const Vector4f& bgcolor, const UiAlignElement& padding, float bevel,
//                                       RectCreateAnchor anchor, const std::string& _vpname, bool centered, bool expand ) {
//    float paddingHeight = std::max( height - padding.Height(), 0.00001f );
//    Vector2f finalSize = size;
//    auto tsize = createText3d( _vpl, msg, Vector2f::ZERO, f, paddingHeight, Color4f::BLACK, msg, true, false );
//    if ( expand ) {
//        if ( tsize.width() + padding.Width() > size.x()) {
//            finalSize = { tsize.width() + padding.Width(), size.y() };
//        }
//    }
//    Vector3f alignPos = Vector3f::ZERO;
//    if ( !centered ) {
//        alignPos = Vector3f{ padding.Left() - finalSize.x() * 0.5f, tsize.height() * 0.5f, 0.0f };
//    } else {
//        alignPos = Vector3f{ finalSize.x() * 0.5f, -finalSize.y() * 0.5f, 0.0f };
//    }
//    createRoundedRect( _vpl, finalSize, startPos, bgcolor, nullptr, 1, bevel, anchor, RectFillMode::Scale,
//                       _vpname + "R" );
//
//    Vector3f alignYText = Vector3f{ -tsize.left(), -tsize.height() - tsize.top(), 0.0f } + alignPos;
//
//    return createText3d( _vpl, msg, Vector3f{ startPos.xy(), startPos.z() + UIZ.level( 1 ) } + alignYText, f,
//                         paddingHeight, color, _vpname, false, centered );
//}

//Rect2f
//createText3d( std::shared_ptr<VPList> _vpl, const std::string& msg, const Vector3f& startPos, const Utility::TTFCore::Font& f,
//              float height, const Vector4f& color, const std::string& _vpname = "", bool bMeasureOnly = false,
//              bool centered = false );
//Rect2f createText3d( std::shared_ptr<VPList> _vpl, const char *msg, const Vector3f& startPos, const Utility::TTFCore::Font& f,
//                     float height, const Vector4f& color, const std::string& _vpname = "", bool bMeasureOnly = false,
//                     bool centered = false );
//
//Rect2f Renderer::createText3d( std::shared_ptr<VPList> _vpl, const std::string& msg, const Vector3f& startPos,
//                               const Utility::TTFCore::Font& f, float height, const Vector4f& color, const std::string& _vpname,
//                               bool bMeasureOnly, bool centered ) {
//    return createText3d( _vpl, msg.c_str(), startPos, f, height, color, _vpname, bMeasureOnly, centered );
//}
//
//Rect2f Renderer::createText3d( std::shared_ptr<VPList> _vpl, const char *msg, const Vector3f& startPos,
//                               const Utility::TTFCore::Font& f, float height, const Vector4f& color,
//                               const std::string& _vpname, bool bMeasureOnly, bool centered ) {

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeText( std::shared_ptr<Font> f ) {
    int32_t numPolysRendered = 0;
    int32_t numTotalPolys = 0;
    int32_t numPolysToDraw = 0;
    Matrix4f tm = Matrix4f::IDENTITY;

    rect = Rect2f::INVALID;

    // Count total number of polygons
    for ( size_t i = 0; i < title.length(); i++ ) {
        Utility::TTF::CodePoint cp( title[i] );
        numTotalPolys += static_cast<int32_t>( f->GetTriCount( cp ));
    }

    float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
    Vector2f gliphSize = Vector2f::ZERO;

    tm.scale( Vector3f( fontHeight, fontHeight, 1.0f ));

    std::shared_ptr<PosTex3dStrip> fs = std::make_shared<PosTex3dStrip>( numTotalPolys, PRIMITIVE_TRIANGLES,
                                                                         VFVertexAllocation::PreAllocate );
    for ( char i : title ) {
        Utility::TTF::CodePoint cp( static_cast<Utility::TTFCore::ulong>(i) );
        const Utility::TTF::Mesh& m = f->GetTriangulation( cp );
        auto numPolysInGlyph = static_cast<int32_t>( m.verts.size());

        // Don't draw an empty space
        if ( numPolysInGlyph > 0 ) {
            if ( numPolysInGlyph + numPolysRendered > numPolysToDraw ) {
                numPolysInGlyph = numPolysToDraw - numPolysRendered;
                if ( numPolysInGlyph & 1 ) ++numPolysInGlyph;
                if ( numPolysInGlyph > static_cast<int32_t>( m.verts.size()))
                    numPolysInGlyph = static_cast<int32_t>( m.verts.size());
            }

            for ( auto vert : m.verts ) {
                gliphSize.setX( JMATH::max( gliphSize.x(), vert.pos.x ));
                gliphSize.setY( JMATH::max( gliphSize.y(), vert.pos.y ));
            }
            for ( auto t = static_cast<int64_t>(m.verts.size() - 1); t >= 0 ; t-- ) {
//            for ( size_t t = 0; t < m.verts.size(); t++ ) {
                Vector4f pos4{ m.verts[t].pos.x / gliphScaler, 1.0f-(m.verts[t].pos.y / gliphScaler), 0.0f, 1.0f };
                pos4 = tm * pos4;
                rect.expand( pos4.xy());
//                pos4.setY( pos4.y() );
                fs->addVertex( XZY::C(pos4.xyz() + orig), Vector2f{ m.verts[t].texCoord, m.verts[t].coef } );
            }

            numPolysRendered += numPolysInGlyph;
        }

        Utility::TTFCore::vec2f kerning = f->GetKerning( Utility::TTF::CodePoint( i ), cp );
        Vector2f nextCharPos = Vector2f( kerning.x / gliphScaler, kerning.y / gliphScaler ) * ( fontHeight );
        tm.translate( nextCharPos );
    }

//    if ( !bMeasureOnly ) {
//        if ( centered ) {
//            for ( int q = 0; q < fs->numVerts; q++ ) {
//                fs->verts[q].pos -= { ret.size() * Vector2f { 0.5f, -0.5f }, 0.0f };
//            }
//        }
//        VPBuilder<FontStrip>{*this}.vl(_vpl).p(fs).m(_vpl->Is2d() ? S::FONT_2D: S::FONT).c(color).n(_vpname).build();
//    }

    size = rect.size();
    return fs;
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeRect( const QuadVertices2& uvm ) {
    return std::make_shared<PosTex3dStrip>( rect, uvm, 0.0f );
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeRoundedRect( [[maybe_unused]] const QuadVertices2& uvm ) {
    // The rect is created from [0,0] to [X, -Y], so from it's top left coordinate downward basically
    std::shared_ptr<PosTex3dStrip> pts;
    const uint32_t numSubDivs = 8;
    const uint32_t numVerts = ( 4 * numSubDivs ) + 1;
    bevelRadius = size.x() < size.y() ? size.x() * bevelRadius : size.y() * bevelRadius;
    Rect2f rect = Rect2f( Vector2f::ZERO, size );
    TextureFillModeScalers tmf = getTextureFillModeScalers( fillMode, size );

    pts = std::make_shared<PosTex3dStrip>( numVerts, PRIMITIVE_TRIANGLE_FAN, VFVertexAllocation::PreAllocate );

    Vector3f pos = Vector3f( bevelRadius, 0.0f, 0.0f );
    pts->addVertex( XZY::C(pos + orig), textureFillModeMapping( rect, pos.xy(), tmf ));
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, 0.0f, static_cast<float>( M_PI_2 ));
        pos = Vector3f( size.x() - bevelRadius, 0.0f, 0.0f ) +
              Vector3f( sinf( angle ), cosf( angle ) - 1.0f, 0.0f ) * bevelRadius;
        pts->addVertex( XZY::C(pos + orig), textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI_2 ), static_cast<float>( M_PI ));
        pos = Vector3f( size.x(), -size.y() + bevelRadius, 0.0f ) +
              Vector3f( sinf( angle ) - 1.0f, cosf( angle ), 0.0f ) * bevelRadius;
        pts->addVertex( XZY::C(pos + orig), textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI ), static_cast<float>( M_PI + M_PI_2 ));
        pos = Vector3f( bevelRadius, -size.y(), 0.0f ) +
              Vector3f( sinf( angle ), cosf( angle ) + 1.0f, 0.0f ) * bevelRadius;
        pts->addVertex( XZY::C(pos + orig), textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI + M_PI_2 ), TWO_PI );
        pos = Vector3f( 0.0f, -bevelRadius, 0.0f ) +
              Vector3f( sinf( angle ) + 1.0f, cosf( angle ), 0.0f ) * bevelRadius;
        pts->addVertex( XZY::C(pos + orig), textureFillModeMapping( rect, pos.xy(), tmf ));
    }

    return pts;
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeLine( const std::vector<Vector3f>& _vlist ) {
    int32_t numVerts = static_cast<int32_t >(_vlist.size()) * 2;
    int32_t numIndices = ( numVerts - 2 ) * 3;
    std::shared_ptr<PosTex3dStrip> colorStrip = std::make_shared<PosTex3dStrip>( numVerts, numIndices,
                                                                                 PRIMITIVE_TRIANGLES,
                                                                                 VFVertexAllocation::PreAllocate );

//    colorStrip->generateStripsFromVerts( extrudePointsWithWidth( _vlist, lineWidth, wrapLine ), wrapLine );
    colorStrip->generateTriangleListFromVerts( extrudePointsWithWidth<ExtrudeStrip>( _vlist, lineWidth, wrapLine ) );

    return colorStrip;
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeLines( const std::vector<std::vector<Vector3f>>& _vlists ) {
    int32_t numVerts = 0;
    int32_t numIndices = 0;
    for ( const auto& vl : _vlists ) {
        numVerts   += static_cast<int32_t >(vl.size()) * 2;
        numIndices += ( numVerts - 2 ) * 3;
    }

    std::shared_ptr<PosTex3dStrip> colorStrip = std::make_shared<PosTex3dStrip>( numVerts, numIndices,
                                                                                 PRIMITIVE_TRIANGLES,
                                                                                 VFVertexAllocation::PreAllocate );

    for ( const auto& vl : _vlists ) {
        colorStrip->generateTriangleListFromVerts( vl );
    }

    return colorStrip;
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeArrow() {
    std::vector<Vector3f> vlistArrowBit;
    std::vector<Vector3f> vlistStem;
    Vector2f n = normalize( outlineVerts[1].xz() - outlineVerts[0].xz() );
    auto pn1 = rotate( n, arrowAngle );
    auto pn2 = rotate( n, -arrowAngle );
    vlistArrowBit.push_back( XZY::C(Vector3f{ outlineVerts[0].xz() + pn1 * arrowLength, zLevel }) );
    vlistArrowBit.push_back( XZY::C(Vector3f{ outlineVerts[0].xz(), zLevel }) );
    vlistArrowBit.push_back( XZY::C(Vector3f{ outlineVerts[0].xz() + pn2 * arrowLength, zLevel }) );

    vlistStem.push_back(outlineVerts[0]);
    vlistStem.push_back(outlineVerts[1]);

//    return makeLine( vlistStem );

    std::vector<std::vector<Vector3f>> cumulativeVlist;
    cumulativeVlist.push_back( extrudePointsWithWidth<ExtrudeStrip>( vlistArrowBit, lineWidth, wrapLine ) );
    cumulativeVlist.push_back( extrudePointsWithWidth<ExtrudeStrip>( vlistStem, lineWidth * 0.66f, wrapLine ) );

    return makeLines( cumulativeVlist );
//    drawLine( _vpl, vlist, color, width, false, 0.0f, percToBeDrawn, _name1 );
//    drawLine( _vpl, p1, p2, color, width * 0.75f, false, 0.0f, percToBeDrawn, _name2 );
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeSeparator() {
    if ( outlineVerts.empty() ) {
        outlineVerts.emplace_back( rect.centreLeft());
        outlineVerts.emplace_back( rect.centreRight());
    }
    return makeLine(outlineVerts);
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makePolygon() {
    // The rect is created from [0,0] to [X, -Y], so from it's top left coordinate downward basically
    TextureFillModeScalers tmf = getTextureFillModeScalers( fillMode, size );

    std::shared_ptr<PosTex3dStrip>pts = std::make_shared<PosTex3dStrip>( triPolyMesh.size(), PRIMITIVE_TRIANGLES,
                                                                         VFVertexAllocation::PreAllocate );

    for ( const auto& v : triPolyMesh ) {
        pts->addVertex( v, textureFillModeMapping( rect, v.xy(), tmf ));
    }

    return pts;
}

void UIShapeBuilder::assemble() {

    if ( orig == Vector2f::HUGE_VALUE_NEG ) {
        orig = rect.origin();
    }
    if ( size == Vector2f::HUGE_VALUE_NEG ) {
        size = rect.size();
    }

    ASSERT(size != Vector2f::ZERO);

    rect = Rect2f{ orig, size, true };

    std::shared_ptr<PosTex3dStrip> vs;

    switch ( shapeType ) {
        case UIShapeType::CameraFrustom2d:
        case UIShapeType::CameraFrustom3d:
            vs = makeLine( outlineVerts );
            break;
        case UIShapeType::Rect2d:
        case UIShapeType::Rect3d: {
            auto rectUVCoords = textureQuadFillModeMapping( fillMode, rect, 1.0f );// sg.RR().RIDM().ip(Name()).getAspectRatio());
            if ( checkBitWiseFlag( effects, UIRenderFlags::RoundedCorners )) {
                vs = makeRoundedRect(rectUVCoords);
            } else {
                vs = makeRect(rectUVCoords);
            }
        }
            break;
        case UIShapeType::Line2d:
        case UIShapeType::Line3d:
            vs = makeLine( outlineVerts );
            break;
        case UIShapeType::Arrow2d:
        case UIShapeType::Arrow3d:
            vs = makeArrow();
            break;
        case UIShapeType::Polygon2d:
        case UIShapeType::Polygon3d:
            vs = makePolygon();
            break;
        case UIShapeType::Text2d:
        case UIShapeType::Text3d: {
            Rect2f textRectOffset{FontUtils::measure( title, sg.FM().get(fontName).get(), fontHeight )};
            orig += Vector2f( -textRectOffset.left(), -textRectOffset.top());
            if ( textAlignment == UiControlFlag::TextAlignRight && size.x() > textRectOffset.width() ) {
                orig.setX( size.x() - textRectOffset.width() + orig.x() );
            } else
            if ( textAlignment == UiControlFlag::TextAlignCenter && size.x() > textRectOffset.width() ) {
                orig.setX( ((size.x() - textRectOffset.width()) * 0.5f) + orig.x() );
            }
            vs = makeText( sg.FM().get(fontName) );
            // Force text aligment to bottom in 2d.
            if ( shapeType == UIShapeType::Text2d ) {
                anchor = RectCreateAnchor::Bottom;
            }
            elem->Name( title );
        }
            break;
        case UIShapeType::Separator2d:
        case UIShapeType::Separator3d:
            vs = makeSeparator();
            break;
    };

    if ( anchor == RectCreateAnchor::Center ) vs->translate( Vector3f( size.x() * -0.5f, size.y() * 0.5f, 0.0f ));
    if ( anchor == RectCreateAnchor::LeftCenter ) vs->translate( Vector3f( 0.0f, size.y() * 0.5f, 0.0f ));
    if ( anchor == RectCreateAnchor::Top ) vs->translate( Vector3f( 0.0f, size.y(), 0.0f ));
    if ( anchor == RectCreateAnchor::Bottom ) vs->translate( Vector3f( 0.0f, -size.y(), 0.0f ));

    // Move to the corresponding zLevel
    vs->translate( Vector3f::UP_AXIS * zLevel );

    // Rotation is used to revolve rects or shapes around a center on the Z axis
    if ( mRot != 0.0f ) {
        Matrix2f mr2;
        mr2.setRotation( mRot );
        Vector3f cr = vs->centre();
        // We can combine this into a concatenation of matrices of course but it might be slower as usually there
        // are very few vertices to transform
        vs->translate( -cr );
        vs->rotate( mr2 );
        vs->translate( cr );
    }

//    ### ref This needs to be here
//    elem->Data()->VertexList(vs);
//    elem->updateTransform();

//    sg.add( elem );
}

bool UIShapeBuilder::validate() const {

    switch ( shapeType ) {
        case UIShapeType::Line2d:
        case UIShapeType::Line3d:
        case UIShapeType::Arrow2d:
        case UIShapeType::Arrow3d:
            if ( outlineVerts.size() < 2 ) return false;
            break;
        case UIShapeType::Polygon2d:
        case UIShapeType::Polygon3d:
            if ( triPolyMesh.size() < 3 ) return false;
            break;
        case UIShapeType::Text2d:
        case UIShapeType::Text3d:
            if ( title.empty() ) return false;
            break;
        default:
            return true;
    };
    return true;
}

std::string UIShapeBuilder::getShaderType( UIShapeType _st ) const {
    auto shaderName = S::TEXTURE_2D;
    switch ( _st ) {
        case UIShapeType::CameraFrustom2d:
        case UIShapeType::CameraFrustom3d:
            break;
        case UIShapeType::Rect2d:
        case UIShapeType::Rect3d:
            shaderName = _st == UIShapeType::Rect2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Line2d:
        case UIShapeType::Line3d:
            shaderName = _st == UIShapeType::Line2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Arrow2d:
        case UIShapeType::Arrow3d:
            shaderName = _st == UIShapeType::Arrow2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Polygon2d:
        case UIShapeType::Polygon3d:
            shaderName = _st == UIShapeType::Polygon2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Text2d:
        case UIShapeType::Text3d:
            shaderName = _st == UIShapeType::Text2d ? S::FONT_2D : S::FONT;
            break;
        case UIShapeType::Separator2d:
        case UIShapeType::Separator3d:
            shaderName = _st == UIShapeType::Separator2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
    }

    return shaderName;
}

void UIShapeBuilder::createDependencyList() {
//    ### Re-add dependency on Font (and everything else)
//    addDependency<Utility::TTFCore::Font, FontBuilder>( fontName, sg.FM() );
//    addDependencies( std::make_shared<UIShapeBuilder>(*this) );
}

void UIShapeBuilder::elemCreate() {
    auto sp = std::make_shared<UIElement>( Name(), shapeType, renderBucketIndex );
//    elem = std::make_shared<UIAsset>( sp, mTransform );
}

UIShapeBuilder& UIShapeBuilder::inj( GeomSP _cloned ) {
//    mTransform = _cloned->getLocalHierTransform();
    return *this;
}
