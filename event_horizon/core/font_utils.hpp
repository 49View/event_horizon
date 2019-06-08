//
// Created by Dado on 2019-03-21.
//

#pragma once

#include <memory>
#include <string>

namespace Utility::TTFCore { class FontInternal;}
using Font = Utility::TTFCore::FontInternal;

namespace JMATH { class Rect2f; }
class Vector2f;

namespace FontUtils {
    JMATH::Rect2f measure( const std::string& msg, const Font* f, float height );
    JMATH::Rect2f optimalSizeForText( const Vector2f& size, const Font* f,
                                      const std::string& text, float& outputHeight );
};



