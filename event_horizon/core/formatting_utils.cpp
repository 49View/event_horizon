#include "formatting_utils.h"
#include "app_globals.h"

const UiAlignElement UiAlignElement::ZERO = UiAlignElement{};

void UiAlignElement::injectIfValid( const UiAlignElement& _rhs ) {
    if ( _rhs.isValid() ) {
        *this = _rhs;
    }
}

UiAlignElement::UiAlignElement( const Vector4f& value ) : mElement( value ) {
}

UiAlignElement::UiAlignElement( const float x, const float y, const float z, const float w ) : mElement{ x,y,z,w } {
}
