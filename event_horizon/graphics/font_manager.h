#pragma once

#include <string>
#include <unordered_map>
#include "core/callback_dependency.h"
#include "TTF.h"
#include "core/math/rect2f.h"
#include "core/htypes_shared.hpp"

class FontManager : public DependencyMaker {
public:
    FontManager() {}
    virtual ~FontManager() {}

    FontManager( FontManager const& ) = delete;
    void operator=( FontManager const& ) = delete;

    DEPENDENCY_MAKER_EXIST(fonts);

    bool create( const uint8_p& ret, const std::string& _indexName );
    const Utility::TTFCore::Font& operator()();
    const Utility::TTFCore::Font& operator[]( const std::string& _fontName );

    static Rect2f measure( const std::string& msg, const Utility::TTFCore::Font& f, float height );
    static Rect2f optimalSizeForText( const Vector2f& size, const Utility::TTFCore::Font& f,
                                      const std::string& text, float& outputHeight );

protected:
    void preCacheFont();

private:
    std::unordered_map<std::string, std::shared_ptr<Utility::TTFCore::Font>> fonts;
};

struct RBUILDER( FontBuilder, fonts, ttf, Binary, BuilderQueryType::Exact )

};
