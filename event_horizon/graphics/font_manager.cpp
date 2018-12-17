#include "font_manager.h"

#include "core/math/matrix4f.h"
#include "core/file_manager.h"

void FontManager::preCacheFont() {
//	font->PreCacheBasicLatin();
}

const Utility::TTFCore::Font& FontManager::operator[]( const std::string& _fontName ) {
    if ( auto ret = fonts.find( _fontName ); ret != fonts.end()) {
        return *ret->second.get();
    }

    // didn't find the font, rollback to defualt
    auto ret = fonts.find( defaultFontName );
    return *ret->second.get();
    // ok default it's not loaded, load it
    //return addFont( FontManager::defaultFont, FontManager::defaultFontName );
}

const Utility::TTFCore::Font& FontManager::operator()() {
    return operator[]( defaultFontName );
}

bool FontManager::create( const uint8_p& ret, const std::string& _indexName ) {
    if ( ret.second > 0 ) {
        fonts[_indexName] = std::make_shared<Utility::TTF::Font>( ret.first.get(), ret.second );
        return true;
    }
    return false;
}

Rect2f FontManager::measure( const std::string& msg, const Utility::TTFCore::Font& f, float height ) {
    int32_t numPolysRendered = 0;
    int32_t numTotalPolys = 0;
    int32_t numPolysToDraw = 0;
    Matrix4f tm = Matrix4f::IDENTITY;
    Rect2f ret = Rect2f::INVALID;

    if ( msg.empty()) {
        return Rect2f::ZERO;
    }

    // Count total number of polygons
    for ( size_t i = 0; i < msg.length(); i++ ) {
        Utility::TTF::CodePoint cp( msg[i] );
        numTotalPolys += static_cast<int32_t>( f.GetTriCount( cp ));
    }

    float gliphScaler = 1000.0f; // It looks like glyphs are stored in a box [1000,1000], so we normalise it to [1,1]
    Vector2f gliphSize = Vector2f::ZERO;

    tm.scale( Vector3f( height, height, 1.0f ));

    for ( size_t i = 0; i < msg.length(); i++ ) {
        Utility::TTF::CodePoint cp( msg[i] );
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
            for ( uint64_t t = 0; t < m.verts.size(); t++ ) {
                Vector4f pos4 = Vector4f( m.verts[t].pos.x / gliphScaler, m.verts[t].pos.y / gliphScaler, 0.0f, 1.0f );
                pos4 = tm * pos4;
                ret.expand( pos4.xy());
            }

            numPolysRendered += numPolysInGlyph;
        }

        Utility::TTFCore::vec2f kerning = f.GetKerning( Utility::TTF::CodePoint( msg[i] ), cp );
        Vector2f nextCharPos = Vector2f( kerning.x / gliphScaler, kerning.y / gliphScaler ) * ( height );
        tm.translate( nextCharPos );
    }

    return ret;
}

Rect2f FontManager::optimalSizeForText( const Vector2f& size, const Utility::TTFCore::Font& f,
                                        const std::string& text, float& outputHeight ) {
    outputHeight = size.y();
    Rect2f lTextRect = FontManager::measure( text.c_str(), f, outputHeight );
    float ratioX = fabs( lTextRect.width() / size.x());
    float ratioY = fabs( lTextRect.height() / size.y());
    if ( ratioX > 1.0f || ratioY > 1.0f ) {
        float ratio = max( ratioX, ratioY );
        outputHeight = size.y() / ratio;
        lTextRect = FontManager::measure( text.c_str(), f, outputHeight );
    }
    return lTextRect;
}

bool FontBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data,[[maybe_unused]]  const DependencyStatus _status ) {
    auto& fm = static_cast<FontManager&>(_md);
    return fm.create( _data, Name() );
}

