#include "ui_render.h"
#include "core/app_globals.h"
#include "ui_shape_builder.h"
#include "ui_control.h"
#include "ui_checkbox.h"
#include "../renderer.h"
//#include "ray_tracing/vertex_buffer.h"

UIRenderFlags UiControlFlagToRenderFlag( const UiControlFlag f ) {
    unsigned int ret = UIRenderFlags::NoEffects;

    if ( checkBitWiseFlag( f, UiControlFlag::HasShadow )) ret |= UIRenderFlags::DropShaodws;
    if ( checkBitWiseFlag( f, UiControlFlag::HasRoundedCorners )) ret |= UIRenderFlags::RoundedCorners;

    return static_cast<UIRenderFlags>( ret );
}

void UiBackgroundRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
        UISB{ UIShapeType::Rect2d }.s(_control->getSize()).c(_control->getMainColor())
                                   .fx(UiControlFlagToRenderFlag(_control->flags()))
                                   .Z(_control->getParams()[1])
                                   .tm(mTransform)
                                   .vn( std::to_string( _control->getId()))
                                   .build(rr);
    }
}

void UiButtonRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
        // Debug rectangle, uncomment to use
//        createRoundedRect(mVPList, _control->getSize(), { 0.0f, 0.0f, _control->getParams()[1] }, Color4f::RED, nullptr);

        float insidePaddingRatio = 0.15f;
        auto al = UiAlignElement{ _control->getSize().x() * insidePaddingRatio,
                        _control->getSize().x() * insidePaddingRatio,
                        _control->getSize().y() * insidePaddingRatio,
                        _control->getSize().y() * insidePaddingRatio };

        if ( nameHasImageExtension( _control->getText())) {
//            auto tb = ImageBuilder{_control->getText()};
//            std::shared_ptr<Texture> tex = rr.T( tb );
//            tex->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
            UISB{ UIShapeType::Rect2d }.s(_control->getSize()).c(_control->getMainColor())
                    .fx(UiControlFlagToRenderFlag(_control->flags()))
                    .t(_control->getText())
                    .s(_control->getSize())
                    .tm(mTransform)
                    .Z(_control->getParams()[1])
                    .build(rr);

//            rr.createRoundedRect( mVPList, _control->getSize(), { 0.0f, 0.0f, _control->getParams()[1] },
//                               _control->getMainColor(), tex );
        }
        if ( !_control->getTitle().empty()) {
            UISB{ UIShapeType::Text2d }.ti(_control->getTitle()).c(_control->getMainColor())
                                     .cb(_control->getBackgroundColor())
                                     .Z(_control->getParams()[1])
                                     .al(al)
                                     .s( _control->getSize())
                                     .f(_control->getFontName())
                                     .fh(_control->getSize().y())
                                     .centred()
                                     .bv(0.15f)
                                     .tm(mTransform)
                                     .vn(_control->getTitle() + std::to_string( unitRand()))
                                     .fx(UiControlFlagToRenderFlag(_control->flags()))
                                     .build(rr);

//            rr.createText3dWithRect( mVPList, _control->getTitle(), { 0.0f, 0.0f, _control->getParams()[1] },
//                                  _control->getSize(), _control->getFont(), _control->getSize().y(),
//                                  _control->getMainColor(),
//                                  _control->getBackgroundColor(),
//                                  0.15f, RectCreateAnchor::Bottom, _control->getTitle() + std::to_string( unitRand()),
//                                  true, false );
        }
    }
}

void UiLabelRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
        UISB{ UIShapeType::Text2d }.ti(_control->getTitle()).c(_control->getMainColor())
                .cb(_control->getBackgroundColor())
                .s( _control->getSize())
                .Z(_control->getParams()[1])
                .f(_control->getFontName())
                .ta(_control->flags())
                .fh(_control->getFontSize())
                .an(RectCreateAnchor::Right)
                .tm(mTransform)
                .vn( std::to_string( _control->getId()))
                .build(rr);

//        UISB{ UIShapeType::Rect2d }.c(_control->getMainColor())
//                .s( _control->getSize())
//                .Z(_control->getParams()[1])
//                .vpl(mVPList)
//                .build(rr);
    }
}

void UiSeparatorRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
        UISB{ UIShapeType::Separator2d }.c(_control->getMainColor())
                .cb(_control->getBackgroundColor())
                .s( _control->getSize())
                .lw( _control->getSize().y() )
                .Z(_control->getParams()[1])
                .tm(mTransform)
                .build(rr);

//        rr.createText3d( mVPList, _control->getTitle(), { 0.0f, 0.0f, _control->getParams()[1] }, _control->getFont(),
//                      _control->getFontSize(), _control->getMainColor());
    }
}

//void UiImageRender::setImage( std::shared_ptr<Texture> source ) {
//    mVPList->setMaterialConstant( UniformNames::colorTexture, source );
//}

void UiImageRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
//        auto tb = ImageBuilder{_control->getText()};
//        std::shared_ptr<Texture> tex = rr.T( tb );
//        tex->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
        RectFillMode fm = _control->hasFlag( UiControlFlag::FillScale ) ? RectFillMode::Scale
                                                                        : RectFillMode::AspectFill;

        UISB{ UIShapeType::Rect2d }.s(_control->getSize()).c(_control->getMainColor())
                .fx(UiControlFlagToRenderFlag(_control->flags()))
                .t(_control->getText())
                .s( _control->getSize())
                .Z(_control->getParams()[1])
                .tm(mTransform)
                .fm(fm)
                .build(rr);

//        rr.createRoundedRect( mVPList, _control->getSize(),
//                           { 0.0f, 0.0f, _control->getParams()[1] }, _control->getMainColor(), tex,
//                           int( UIRenderFlags::NoEffects ),
//                           _control->hasFlag( UiControlFlag::HasRoundedCorners ) ? 0.02f : 0.0f,
//                           RectCreateAnchor::Bottom, fm );
    }
}

void UiInputBoxRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
//        rr.createText3d( mVPList, _control->getTitle(), { 0.0f, 0.0f, _control->getParams()[1] }, _control->getFont(),
//                      _control->getFontSize(), _control->getMainColor());
    }
}

void UiCheckBoxRender::make() {
    auto _control = mControl.lock();
    if ( _control ) {
//        UiCheckBox *checkbox_control = dynamic_cast<UiCheckBox *>(_control.get());
//        Rect2f lTextRect = rr.createText3d( mVPList, _control->getText(), { 0.0f, 0.0f, _control->getParams()[1] },
//                                         _control->getFont(),
//                                         _control->getFontSize(), _control->getMainColor());
//
//        rr.createRoundedRect( mVPList,
//                           { lTextRect.height(), lTextRect.height() * 0.5f },
//                           Vector3f{ checkbox_control->getSliderBallOnLeftValue(), 0.0f, _control->getParams()[1] },
//                           Color4f::BLUE_SHADOW.A( 0.75f ),
//                           nullptr, 0, 0.5f, RectCreateAnchor::Top );
//
//        auto tb = ImageBuilder{  "ui_slider_ball.png" };
//        rr.createRoundedRect( mVPList,
//                           { lTextRect.height() * 0.5f, lTextRect.height() * 0.5f },
//                           Vector3f{ checkbox_control->State() ? checkbox_control->getSliderBallOffRightValue() :
//                                     checkbox_control->getSliderBallOnLeftValue(), 0.0f, UIZ.level( 4 ) },
//                           Color4f::WHITE, rr.T( tb ), 0, 0.5f, RectCreateAnchor::Top,
//                           RectFillMode::AspectFill, std::to_string( _control->getId()) + "state" );
    }
}

void UiSliderRender::make() {
//    auto tb1 = ImageBuilder{ "ui_slider_bg_left.png" };
//    ui_slider_bg_left   = rr.T( tb1 );
//    auto tb2 = ImageBuilder{  "ui_slider_bg_right.png" };
//    ui_slider_bg_right  = rr.T( tb2 );
//    auto tb3 = ImageBuilder{ "ui_slider_cap_left.png" };
//    ui_slider_cap_left  = rr.T( tb3 );
//    auto tb4 = ImageBuilder{ "ui_slider_cap_right.png" };
//    ui_slider_cap_right = rr.T( tb4 );
//    auto tb5 = ImageBuilder{ "ui_slider_ball.png" };
//    ui_slider_ball      = rr.T( tb5 );
//
//    ui_slider_bg_left->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
//    ui_slider_bg_right->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
//    ui_slider_cap_left->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
//    ui_slider_cap_right->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
//    ui_slider_ball->setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );

    //setSliderBarTo( 0.5f );
    //mDisplayListStatic->createRect(Rect2f{{0.0f, -mSize->value.y()*0.5f}, {mSize->value.y(),mSize->value.y()}, true}, mainColor, rr.T("ui_slider_ball.png"));
    //    setSliderBarTo( lerpInv( static_cast<float>( mCurrValue ), static_cast<float>( mStartValue ), static_cast<float>( mEndValue ) ) );
}

//void UiSlider::setSliderBarTo( float value ) {
////	mDisplayListStatic->reset();
////	createRect( mDisplayListStatic, Rect2f{ { 0.0f, -mSize->value.y() },{ mSize->value.x()*0.5f,mSize->value.y() }, true }, mMainColor->value, ui_slider_bg_left, RectFillMode::AspectFillLeft, mZ );
////	createRect( mDisplayListStatic, Rect2f{ {mSize->value.x()*0.5f, -mSize->value.y()}, {mSize->value.x()*0.5f,mSize->value.y()}, true }, mMainColor->value, ui_slider_bg_right, RectFillMode::AspectFillRight, mZ );
////	float minBarSize = max( mSize->value.x()*value - mSize->value.y()*0.5f, mSize->value.y()*0.5f );
////	float minBarOffset = max( -mSize->value.y()*0.5f + mSize->value.x()*value, -mSize->value.y()*0.5f );
////	createRect( mDisplayListStatic, Rect2f{ { 0.0f, -mSize->value.y() },{ minBarSize ,mSize->value.y() }, true }, mMainColor->value, ui_slider_cap_left, RectFillMode::AspectFillLeft, mZ );
////	createRect( mDisplayListStatic, Rect2f{ {minBarOffset, -mSize->value.y() },{ mSize->value.y(),mSize->value.y() }, true }, mMainColor->value, ui_slider_ball, RectFillMode::Scale, mZ );
//}

void UiControlRenderer::UINormalAxis( const Vector3f& val ) {
    mUINormalAxis = val;
    origRot = Matrix4f::IDENTITY;
    origRot.setFromRotationAnglePos( UINormalAxis(), Vector3f::Z_AXIS, Vector3f::ZERO );
}

void UiControlRenderer::OrigRot( const Matrix4f& val ) {
    origRot = val;
}

//std::shared_ptr<VertexBuffer> UiControlRenderer::createVertexBuffer( const std::unique_ptr<UiControl>& _control ) {
//    mVertexBuffer = std::make_shared<QuadVertexBuffer>();
//
//    //initial values
//    Vector2f val = _control->getSize();
//
//    std::vector<Vector4f> vertices;
//
//    float anchorOrientation = -1.0f;
//
//    //topLeft
//    vertices.push_back( Vector4f( val * Vector2f( 0.0f, 0.0f ), 0.0f, 1.0f ));
//    //topRight
//    vertices.push_back( Vector4f( val * Vector2f( 1.0f, 0.0f ), 0.0f, 1.0f ));
//    //bottomRight
//    vertices.push_back( Vector4f( val * Vector2f( 1.0f, 1.0f * anchorOrientation ), 0.0f, 1.0f ));
//    //bottomLeft
//    vertices.push_back( Vector4f( val * Vector2f( 0.0f, 1.0f * anchorOrientation ), 0.0f, 1.0f ));
//
//    mVertexBuffer->initialVerticeValues( vertices );
//    return mVertexBuffer;
//}

void UiControlRenderer::update( [[maybe_unused]] const MatrixAnim& dadTransform ) {

    auto _control = mControl.lock();

    if ( _control ) {
        Matrix4f& lTransform = *mTransform.get();

        Vector3f lPos = _control->getPosition();
        lTransform.identityWithPos( lPos );

        lTransform.mult(Matrix4f(mAnimMatrix));

//        mVPList->setTransform( mTransform );
    }
}

UiControlRenderer::~UiControlRenderer() {

}

void UiTextInputRenderer::printTextInput( const std::string& _title, const std::string& _text,
                                          [[maybe_unused]] const Utility::TTFCore::Font& _font) {
    float tsize = 0.25f;
    float tcenter = 0.5f;
    float tpad = 0.01f;
    float fontHeight = .03f;
    UiAlignElement padding{ tpad * getScreenAspectRatio, tpad * getScreenAspectRatio, fontHeight * 0.15f,
                            fontHeight * 0.15f };

    UISB{ UIShapeType::Text2d }.ti(_title + _text + " |").c(Color4f::WHITE)
            .cb(Color4f::PASTEL_CYAN)
            .origin({ tcenter * getScreenAspectRatio, tcenter})
            .s(Vector2f{ tsize, fontHeight })
            .ZLevel(41)
            .al(padding)
            .bv(0.03f)
            .f(defaultFontName)
            .fh(fontHeight)
            .an(RectCreateAnchor::Center)
            .vn(_title)
            .expandable()
            .build(rr);

//    rr.createText3dWithRect( mVPList, _title + _text + " |",
//                          Vector3f{ tcenter * getScreenAspectRatio, tcenter, UIZ.level( 41 ) },
//                          Vector2f{ tsize, fontHeight }, _font, fontHeight, Color4f::WHITE,
//                          Color4f::PASTEL_CYAN, padding, 0.03f, RectCreateAnchor::Center, _title,
//                          false, true );
}

void UiTextInputRenderer::clear() {
//    mVPList->reset();
}

UiTextInputRenderer::UiTextInputRenderer( Renderer& _rr ) : rr(_rr) {
//    mVPList = generate2dVPList( rr, "UiTextInput2dMiddleScreen" );
}
