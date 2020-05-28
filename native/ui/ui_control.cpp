#include "ui_control.h"

#include "core/app_globals.h"

UiControl::UiControl( FontManager& fm ) : fm( fm ) {
    static uint64_t mNextId{ 0 };
    mId = ++mNextId;
}

UiControlPosition UiControl::stringToUiControlPosition( const std::string& _value ) {
    if ( _value == "ContinueX" ) return UiControlPosition::ContinueX;
    if ( _value == "ContinueY" ) return UiControlPosition::ContinueY;
    if ( _value == "Overlap" ) return UiControlPosition::Overlap;

    return UiControlPosition::Invalid;
}

UiControlSizeType UiControl::stringToUiControlSizeType( const std::string& _value ) {
    if ( _value == "FullSize" ) return UiControlSizeType::FullSize;
    if ( _value == "FullQuad" ) return UiControlSizeType::FullQuad;
    if ( _value == "FullWidth" ) return UiControlSizeType::FullWidth;
    if ( _value == "FullHeight" ) return UiControlSizeType::FullHeight;
    if ( _value == "Explicit" ) return UiControlSizeType::Explicit;

    return UiControlSizeType::Invalid;
}

UiControlFlag UiControl::stringToUiControlFlag( const std::string& _value ) {
    if ( _value == "UseScreenSpace" ) return UiControlFlag::UseScreenSpace;
    if ( _value == "HasRoundedCorners" ) return UiControlFlag::HasRoundedCorners;
    if ( _value == "TextAlignCenter" ) return UiControlFlag::TextAlignCenter;
    if ( _value == "TextAlignLeft" ) return UiControlFlag::TextAlignLeft;
    if ( _value == "TextAlignRight" ) return UiControlFlag::TextAlignRight;
    if ( _value == "FillScale" ) return UiControlFlag::FillScale;
    if ( _value == "FloatLeft" ) return UiControlFlag::FloatLeft;
    if ( _value == "FloatRight" ) return UiControlFlag::FloatRight;
    if ( _value == "IsArray" ) return UiControlFlag::IsArray;

    return UiControlFlag::Invalid;
}

TouchResult UiControl::checkTouchDown( const Vector2f& pos ) const {
    TouchResult hit = TOUCH_RESULT_NOT_HIT;

    if ( !mEnabled || !mVisible || !mIsClickable || !mAcceptInputs ) {
        hit = TOUCH_RESULT_NOT_HIT;
    } else {
        if ( IsScreenSpace()) {
            auto screenPos = AG.getAspectRatio2dFromUnnormalizedScreenPos( pos );
            auto r = Rect2d();
            hit = r.contains( screenPos ) ? TOUCH_RESULT_HIT
                                          : TOUCH_RESULT_NOT_HIT;
        } else {
            // TODO: Reenable this for 3d
            //hit = Rect().contains( MI.PlaneTouchDownPos3d().xy()) ? TOUCH_RESULT_HIT : TOUCH_RESULT_NOT_HIT;
        }
    }

    return hit;
}

void UiControl::touchDownAnimation() {
    AcceptInputs( false );
//    animateTo( mScale, V2fc::ONE * 0.95f, 0.05f, 0.0f );
}

void UiControl::touchUpAnimation() {
    AcceptInputs( true );
//    animateTo( mScale, V2fc::ONE, 0.05f, 0.0f );
}

void UiControl::addFlag( UiControlFlag flag ) {
    mFlags = mFlags | flag;
}

void UiControl::removeFlag( UiControlFlag flag ) {
    mFlags = mFlags & ( ~flag );
}

void UiControl::setEnabled( bool enabled ) {
    mEnabled = enabled;
    mEnableAlphaMult = mEnabled ? 1.0f : 0.2f;
//	alphaRenderLambda();
}

//void UiControl::mainColorRenderLambda() {
//	mDisplayListStatic->setMaterialConstant( UniformNames::color, mMainColor->value.xyz() );
//}
//
//void UiControl::alphaRenderLambda() {
//	mDisplayListStatic->setMaterialConstant( UniformNames::alpha, mAlpha->value );
//	mDisplayListStatic->setMaterialConstant( UniformNames::opacity, mEnableAlphaMult );
//	setVisible( ( mAlpha->value * mEnableAlphaMult != 0.0f ) );
//}

void UiControl::changeMainColor( [[maybe_unused]] const Color4f& colorTo, [[maybe_unused]] float time /*= 0.0f*/ ) {
//	animateTo( mMainColor, colorTo, time, 0.0f, std::bind( &UiControl::mainColorRenderLambda, this ) );
}

void UiControl::changeAlpha( [[maybe_unused]]const float alphaTo, [[maybe_unused]] float time /*= 0.0f*/ ) {
//	animateFromTo( mAlpha, mAlpha->value, alphaTo, time, 0.0f, std::bind( &UiControl::alphaRenderLambda, this ) );
}

Vector3f UiControl::BaricentricHit( const Vector2f& pos ) const {
    return lerpInv( AG.getAspectRatio2dFromUnnormalizedScreenPos( pos ), Rect2d().topLeft(), Rect2d().bottomRight());
}

bool UiControl::isOpaque() const {
    return ( mMainColor.w() >= 1.0f );
}

void UiControl::init( const UiControlBuilder& _data ) {
    mFlags = _data.flags;

    mIsScreenSpace = checkBitWiseFlag( mFlags, UiControlFlag::UseScreenSpace );

    mName = _data.cname;
    mPosition = _data.pos;
    mSize = _data.size;
    mMainColor = _data.mainColor;
    mBackgroundColor = _data.backgroundColor.x() >= 0.0f ? _data.backgroundColor : mMainColor;
    mText = _data.text;
    mTitle = _data.title;
    mFontName = _data.font.empty() ? defaultFontName : _data.font;
    mParams = _data.params;

    initImpl();
}

void UiControl::alignOn( const Rect2f& lTextRect ) {
    // Alignment
    int align = ( mFlags & UiControlFlag::TextAlignCenter ) | ( mFlags & UiControlFlag::TextAlignLeft ) |
                ( mFlags & UiControlFlag::TextAlignRight );
    switch ( align ) {
        case UiControlFlag::TextAlignCenter:
            if ( mSize.x() > lTextRect.width()) {
                mPosition.incX(( mSize.x() - lTextRect.width()) * 0.5f );
            }
            break;
        case UiControlFlag::TextAlignLeft:
            break;
        case UiControlFlag::TextAlignRight:
            mPosition.incX(( mSize.x() - lTextRect.width()));
            break;
        default:
            break;
    }
    // ### this is tricky: the apparently incomprehensible "lTextRect.top()" bit is because the text rect doesn't start from zero
    // but, apart that it's inverted so top == bottom, it could start from below the baseline (which is always at zero)
    // so for example, a "g" will have the bottom part of the letter below the baseline hence having a bottom negative.
    // In order to center the text as a whole vertically then we need to add that offset back to the rect, hence the lTextRect.top() bit.
    // Same goes for the left hand side of the text, it could start from the left of zero with some letters like "f" for example or simply font spacing
    mPosition.incX( -lTextRect.left());
    mPosition.incY( -lTextRect.top() - mSize.y() * 0.5f - fabs( lTextRect.height() * 0.5f ));
}

bool UiControl::hasFlag( UiControlFlag flag ) const {
    return checkBitWiseFlag( mFlags, flag );
}

UiControlFlag UiControl::flags() const {
    return static_cast<UiControlFlag>(mFlags);
}

