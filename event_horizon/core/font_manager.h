#pragma once

#include <string>
#include <unordered_map>
#include <core/builders.hpp>
#include <core/TTF.h>
#include <core/math/rect2f.h>

class FontManager : public ResourceManager<Utility::TTFCore::Font> {
public:
    const Utility::TTFCore::Font& operator()();
    const Utility::TTFCore::Font& operator[]( const std::string& _fontName );

    static Rect2f measure( const std::string& msg, const Utility::TTFCore::Font& f, float height );
    static Rect2f optimalSizeForText( const Vector2f& size, const Utility::TTFCore::Font& f,
                                      const std::string& text, float& outputHeight );

protected:
    void preCacheFont();
};

class FontBuilder : public ResourceBuilder<Utility::TTFCore::Font, FontManager> {
public:
    explicit FontBuilder( FontManager& _mm ) : ResourceBuilder(_mm) {}
};

//struct RBUILDER( FontBuilder, fonts, ttf, Binary, BuilderQueryType::Exact, 0 )};
