//
// Created by Dado on 2019-03-21.
//

#include "font_utils.hpp"

#include <cstdint>
#include <core/math/rect2f.h>
#include <core/math/matrix4f.h>
#include <core/TTF.h>

namespace FontUtils {

    Rect2f measure( const std::string& msg, const Font *f, float height ) {
        auto frect = f->GetMasterRect();
        Rect2f ret{ Rect2f::INVALID };
        V2f fscale = V2f{ 1.0f / ( frect.z - frect.x ), 1.0f / ( frect.w - frect.y ) } * height;
        Utility::TTFCore::vec2f kerning{ 0.0f, 0.0f };
        for ( size_t t = 0; t < msg.size(); t++ ) {
            char i = msg[t];
            Utility::TTF::CodePoint cp(static_cast<Utility::TTFCore::ulong>(i));
            auto grect = f->GetGlyphRect(cp);
            ret.merge(Rect2f{ V2f{ kerning.x + grect.x, grect.y } * fscale,
                              V2f{ kerning.x + grect.z, grect.w } * fscale });
            if ( t < msg.size() - 1 ) {
                kerning += f->GetKerning(cp, Utility::TTF::CodePoint(msg[t + 1]));
            }
        }
        return ret;
    }

    Rect2f optimalSizeForText( const Vector2f& size,
                               const Font *f,
                               const std::string& text,
                               float& outputHeight ) {

        outputHeight = size.y();
        Rect2f lTextRect{ measure(text.c_str(), f, outputHeight) };
        float ratioX = fabs(lTextRect.width() / size.x());
        float ratioY = fabs(lTextRect.height() / size.y());
        if ( ratioX > 1.0f || ratioY > 1.0f ) {
            float ratio = max(ratioX, ratioY);
            outputHeight = size.y() / ratio;
            lTextRect = Rect2f{ measure(text.c_str(), f, outputHeight) };
        }
        return lTextRect;
    }

    V2f fitTextInBox( const Font *font, const std::string& text, const Rect2f& bbox, float& fontHeight ) {
        auto textRect = FontUtils::measure(text, font, fontHeight);
        if ( bbox.width() < textRect.width() ) {
            float ratio = ( textRect.width() / bbox.width() ) * 1.2f; // add text ratio with some slack
            fontHeight /= ratio;
            textRect = FontUtils::measure(text, font, fontHeight);
        }
        float textStartOffset = ( bbox.width() - textRect.width() ) * 0.5f;
        return bbox.centreLeft() + V2fc::X_AXIS * textStartOffset;
    }

}