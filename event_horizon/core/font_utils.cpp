//
// Created by Dado on 2019-03-21.
//

#include "font_utils.hpp"

#include <cstdint>
#include <core/math/rect2f.h>
#include <core/math/matrix4f.h>
#include <core/TTF.h>

namespace FontUtils {

Rect2f measure( const std::string& msg, std::shared_ptr<Font> f, float height ) {
    int32_t numPolysRendered = 0;
    int32_t numTotalPolys = 0;
    int32_t numPolysToDraw = 0;
    Matrix4f tm = Matrix4f::IDENTITY;
    Rect2f ret = Rect2f::INVALID;

    if ( msg.empty()) {
        return Rect2f::ZERO;
    }

    // Count total number of polygons
    for ( char i : msg ) {
        Utility::TTF::CodePoint cp( i );
        numTotalPolys += static_cast<int32_t>( f->GetTriCount( cp ));
    }

    float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
    Vector2f gliphSize = Vector2f::ZERO;

    tm.scale( Vector3f( height, height, 1.0f ));

    for ( char i : msg ) {
        Utility::TTF::CodePoint cp( i );
        const Utility::TTF::Mesh& m = f->GetTriangulation( cp );
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
            for ( uint64_t t = 0; t < m.verts.size(); t++ ) {
                Vector4f pos4 = Vector4f( m.verts[t].pos.x / gliphScaler, m.verts[t].pos.y / gliphScaler, 0.0f, 1.0f );
                pos4 = tm * pos4;
                ret.expand( pos4.xy());
            }

            numPolysRendered += numPolysInGlyph;
        }

        Utility::TTFCore::vec2f kerning = f->GetKerning( Utility::TTF::CodePoint( i ), cp );
        Vector2f nextCharPos = Vector2f( kerning.x / gliphScaler, kerning.y / gliphScaler ) * ( height );
        tm.translate( nextCharPos );
    }

    return ret;
}

Rect2f optimalSizeForText( const Vector2f& size,
                           std::shared_ptr<Font> f,
                           const std::string& text,
                           float& outputHeight ) {

    outputHeight = size.y();
    Rect2f lTextRect = measure( text.c_str(), f, outputHeight );
    float ratioX = fabs( lTextRect.width() / size.x());
    float ratioY = fabs( lTextRect.height() / size.y());
    if ( ratioX > 1.0f || ratioY > 1.0f ) {
        float ratio = max( ratioX, ratioY );
        outputHeight = size.y() / ratio;
        lTextRect = measure( text.c_str(), f, outputHeight );
    }
    return lTextRect;
}

}