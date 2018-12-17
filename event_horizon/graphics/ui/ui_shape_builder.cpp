//
// Created by Dado on 06/01/2018.
//

#include "ui_shape_builder.h"
#include "core/image_builder.h"
#include "../renderer.h"
#include "../font_manager.h"

typedef std::pair<Vector2f, Vector2f> TextureFillModeScalers;

uint64_t UIShapeBuilder::sid = 0;

TextureFillModeScalers
getTextureFillModeScalers( const RectFillMode fm, const Vector2f& size, float _aspectRation ) {
    // Those 2 variables handle basically a "AspectFit" scaling, in which the image is scaled in a way the whole image is present maintaining the aspect ratio.
    Vector2f texAspectVector = size.vectorRatio();
    Vector2f texAspectVectorOffset = Vector2f::ZERO;

    if ( fm == RectFillMode::Scale ) {
        texAspectVector = Vector2f::ONE;
    }

    if ( fm == RectFillMode::AspectFill ) {
        float aoh = _aspectRation;
        float sar = size.ratio();
        if ( aoh > sar ) {
            texAspectVector = Vector2f{ (1.0f/aoh) * getScreenAspectRatio, 1.0f };
            texAspectVectorOffset = { (aoh / getScreenAspectRatio) * 0.5f + 0.5f, 0.0f };
        } else {
            texAspectVector = Vector2f{ 1.0f, aoh / (getScreenAspectRatio) };
            texAspectVectorOffset = { 0.0f, (aoh / getScreenAspectRatio) * -0.5f + 0.5f };
        }
    }
    if ( fm == RectFillMode::AspectFit || fm == RectFillMode::AspectFitLeft ||
         fm == RectFillMode::AspectFitRight ) {
        float sizeTexRatio = size.ratio() / _aspectRation;
        if ( sizeTexRatio > 1.0f ) {
            texAspectVector = { sizeTexRatio, 1.0f };
            texAspectVectorOffset = { sizeTexRatio * -0.5f + 0.5f, 0.0f };
            if ( fm == RectFillMode::AspectFitLeft ) texAspectVectorOffset = Vector2f::ZERO;
            if ( fm == RectFillMode::AspectFitRight )
                texAspectVectorOffset.setX( texAspectVectorOffset.x() * 2.0f );
        } else {
            float invSizeTexRatio = 1.0f / sizeTexRatio;
            texAspectVector = { 1.0f, invSizeTexRatio };
            texAspectVectorOffset = { 0.0f, ( invSizeTexRatio * -0.5f + 0.5f ) };
        }
    }

    return std::make_pair( texAspectVector, texAspectVectorOffset );
}

TextureFillModeScalers
getTextureFillModeScalers( const RectFillMode fm, const Vector2f& size ) {
    // Those 2 variables handle basically a "AspectFit" scaling, in which the image is scaled in a way the whole image is present maintaining the aspect ratio.
    Vector2f texAspectVector = Vector2f::ONE;// size.vectorRatio();
    Vector2f texAspectVectorOffset = Vector2f::ZERO;

    switch ( fm ) {
        case RectFillMode::Scale:
            texAspectVector = Vector2f::ONE;
            break;
        case RectFillMode::AspectFit:
        case RectFillMode::AspectFill:
        case RectFillMode::AspectFitLeft:
        case RectFillMode::AspectFitRight:
        case RectFillMode::AspectFitTop:
        case RectFillMode::AspectFitBottom:
            texAspectVector = size.vectorRatio();
            break;
        default:
            break;
    }

    return std::make_pair( texAspectVector, texAspectVectorOffset );
}

Vector2f textureFillModeMapping( const Rect2f& rect, const Vector2f pos, TextureFillModeScalers tfms ) {
    return rect.normalizeWithinRect( pos ) * tfms.first + tfms.second;
}

QuadVertices2
textureQuadFillModeMapping( const RectFillMode fm, const Rect2f& rect, float _imageAspectRatio ) {
    TextureFillModeScalers tmfs = getTextureFillModeScalers( fm, rect.size(), _imageAspectRatio );
    QuadVertices2 qvt(
            textureFillModeMapping( rect, rect.topRight(), tmfs ),
            textureFillModeMapping( rect, rect.bottomRight(), tmfs ),
            textureFillModeMapping( rect, rect.topLeft(), tmfs ),
            textureFillModeMapping( rect, rect.bottomLeft(), tmfs )
    );
    return qvt;
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

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeText( const Utility::TTFCore::Font& f ) {
    int32_t numPolysRendered = 0;
    int32_t numTotalPolys = 0;
    int32_t numPolysToDraw = 0;
    Matrix4f tm = Matrix4f::IDENTITY;

    if ( title.empty() ) {
        rect = Rect2f::ZERO;
        return std::make_shared<PosTex3dStrip>( 0, PRIMITIVE_TRIANGLES, VFVertexAllocation::PreAllocate );
    }

    rect = Rect2f::INVALID;

    // Count total number of polygons
    for ( size_t i = 0; i < title.length(); i++ ) {
        Utility::TTF::CodePoint cp( title[i] );
        numTotalPolys += static_cast<int32_t>( f.GetTriCount( cp ));
    }

    float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
    Vector2f gliphSize = Vector2f::ZERO;

    tm.scale( Vector3f( fontHeight, fontHeight, 1.0f ));

    std::shared_ptr<PosTex3dStrip> fs = std::make_shared<PosTex3dStrip>( numTotalPolys, PRIMITIVE_TRIANGLES,
                                                                         VFVertexAllocation::PreAllocate );
    for ( size_t i = 0; i < title.length(); i++ ) {
        Utility::TTF::CodePoint cp( title[i] );
        const Utility::TTF::Mesh& m = f.GetTriangulation( cp );
        int32_t numPolysInGlyph = static_cast<int32_t>( m.verts.size());

        // Don't draw an empty space
        if ( numPolysInGlyph > 0 ) {
            if ( numPolysInGlyph + numPolysRendered > numPolysToDraw ) {
                numPolysInGlyph = numPolysToDraw - numPolysRendered;
                if ( numPolysInGlyph & 1 ) ++numPolysInGlyph;
                if ( numPolysInGlyph > static_cast<int32_t>( m.verts.size()))
                    numPolysInGlyph = static_cast<int32_t>( m.verts.size());
            }

            for ( uint64_t t = 0; t < m.verts.size(); t++ ) {
                gliphSize.setX( JMATH::max( gliphSize.x(), m.verts[t].pos.x ));
                gliphSize.setY( JMATH::max( gliphSize.y(), m.verts[t].pos.y ));
            }
            for ( int64_t t = m.verts.size()-1; t >= 0 ; t-- ) {
                Vector4f pos4{ m.verts[t].pos.x / gliphScaler, 1.0f-(m.verts[t].pos.y / gliphScaler), 0.0f, 1.0f };
                pos4 = tm * pos4;
                rect.expand( pos4.xy());
                //				pos4.setY( pos4.y() - height );
                fs->addVertex( XZY::C(pos4.xyz() + orig), Vector2f{ m.verts[t].texCoord, m.verts[t].coef } );
            }

            numPolysRendered += numPolysInGlyph;
        }

        Utility::TTFCore::vec2f kerning = f.GetKerning( Utility::TTF::CodePoint( title[i] ), cp );
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

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeRect(const QuadVertices2& uvm) {
    return std::make_shared<PosTex3dStrip>( Rect2f{ rect.origin() - Vector2f{0.0f, size.y()}, rect.size(), true },
                                            uvm, 0.0f );
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
    pts->addVertex( pos + orig, textureFillModeMapping( rect, pos.xy(), tmf ));
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, 0.0f, static_cast<float>( M_PI_2 ));
        pos = Vector3f( size.x() - bevelRadius, 0.0f, 0.0f ) +
              Vector3f( sinf( angle ), cosf( angle ) - 1.0f, 0.0f ) * bevelRadius;
        pts->addVertex( pos + orig, textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI_2 ), static_cast<float>( M_PI ));
        pos = Vector3f( size.x(), -size.y() + bevelRadius, 0.0f ) +
              Vector3f( sinf( angle ) - 1.0f, cosf( angle ), 0.0f ) * bevelRadius;
        pts->addVertex( pos + orig, textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI ), static_cast<float>( M_PI + M_PI_2 ));
        pos = Vector3f( bevelRadius, -size.y(), 0.0f ) +
              Vector3f( sinf( angle ), cosf( angle ) + 1.0f, 0.0f ) * bevelRadius;
        pts->addVertex( pos + orig, textureFillModeMapping( rect, pos.xy(), tmf ));
    }
    for ( uint32_t t = 0; t < numSubDivs; t++ ) {
        float delta = static_cast<float>( t ) / static_cast<float>( numSubDivs - 1 );
        float angle = JMATH::lerp( delta, static_cast<float>( M_PI + M_PI_2 ), TWO_PI );
        pos = Vector3f( 0.0f, -bevelRadius, 0.0f ) +
              Vector3f( sinf( angle ) + 1.0f, cosf( angle ), 0.0f ) * bevelRadius;
        pts->addVertex( pos + orig, textureFillModeMapping( rect, pos.xy(), tmf ));
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
    colorStrip->generateTriangleListFromVerts( extrudePointsWithWidth( _vlist, lineWidth, wrapLine ) );

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
    cumulativeVlist.push_back( extrudePointsWithWidth( vlistArrowBit, lineWidth, wrapLine ) );
    cumulativeVlist.push_back( extrudePointsWithWidth( vlistStem, lineWidth * 0.66f, wrapLine ) );

    return makeLines( cumulativeVlist );
//    drawLine( _vpl, vlist, color, width, false, 0.0f, percToBeDrawn, _name1 );
//    drawLine( _vpl, p1, p2, color, width * 0.75f, false, 0.0f, percToBeDrawn, _name2 );
}

std::shared_ptr<PosTex3dStrip> UIShapeBuilder::makeSeparator() {
    if ( outlineVerts.empty() ) {
        outlineVerts.push_back(rect.centreLeft());
        outlineVerts.push_back(rect.centreRight());
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

void UIShapeBuilder::assemble( DependencyMaker& _md ) {

    Renderer& rr = static_cast<Renderer&>( _md );

    if ( orig == Vector2f::HUGE_VALUE_NEG ) {
        orig = rect.origin();
    }
    if ( size == Vector2f::HUGE_VALUE_NEG ) {
        size = rect.size();
    }

    ASSERT(size != Vector2f::ZERO);

    rect = Rect2f{ orig, size, true };

    auto vpList = rr.VPL( CommandBufferLimits::UIStart + renderBucketIndex, vname, mTransform, color.w() );

    std::shared_ptr<PosTex3dStrip> vs;

    auto shaderName = S::TEXTURE_2D;
    switch ( shapeType ) {
        case UIShapeType::CameraFrustom2d:
        case UIShapeType::CameraFrustom3d:
            vs = makeLine( outlineVerts );
            shaderName = shapeType == UIShapeType::Line2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;

        case UIShapeType::Rect2d:
        case UIShapeType::Rect3d: {
            shaderName = shapeType == UIShapeType::Rect2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            auto rectUVCoords = textureQuadFillModeMapping( fillMode, rect, rr.RIDM().ip(Name()).getAspectRatio());
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
            shaderName = shapeType == UIShapeType::Line2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Arrow2d:
        case UIShapeType::Arrow3d:
            vs = makeArrow();
            shaderName = shapeType == UIShapeType::Arrow2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Polygon2d:
        case UIShapeType::Polygon3d:
            vs = makePolygon();
            shaderName = shapeType == UIShapeType::Polygon2d ? S::TEXTURE_2D : S::TEXTURE_3D;
            break;
        case UIShapeType::Text2d:
        case UIShapeType::Text3d: {
            Rect2f textRectOffset = rr.FM().measure( title, rr.FM()[fontName], fontHeight );
            orig += Vector2f( -textRectOffset.left(), -textRectOffset.top());
            if ( textAlignment == UiControlFlag::TextAlignRight && size.x() > textRectOffset.width() ) {
                orig.setX( size.x() - textRectOffset.width() + orig.x() );
            } else
            if ( textAlignment == UiControlFlag::TextAlignCenter && size.x() > textRectOffset.width() ) {
                orig.setX( ((size.x() - textRectOffset.width()) * 0.5f) + orig.x() );
            }
            vs = makeText( rr.FM()[fontName] );
            shaderName = shapeType == UIShapeType::Text2d ? S::FONT_2D : S::FONT;
            // Force text aligment to bottom in 2d.
            if ( shapeType == UIShapeType::Text2d ) {
                anchor = RectCreateAnchor::Bottom;
            }
        }
            break;
        case UIShapeType::Separator2d:
        case UIShapeType::Separator3d:
            vs = makeSeparator();
            shaderName = shapeType == UIShapeType::Separator2d ? S::TEXTURE_2D : S::TEXTURE_3D;
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

    VPBuilder<PosTex3dStrip>{rr}.vl(vpList).p(vs).s(shaderName).t(tname).c(color).n(vname).build();
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

void UIShapeBuilder::createDependencyList( DependencyMaker& _md ) {

    Renderer& rr = static_cast<Renderer&>(_md);
    addDependency<ImageBuilder>( tname, rr.RIDM() );
    addDependency<FontBuilder>( fontName, rr.FM() );

    addDependencies( std::make_shared<UIShapeBuilder>(*this), _md );
}

