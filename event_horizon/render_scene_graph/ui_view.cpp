//
// Created by Dado on 2019-05-28.
//

#include "ui_view.hpp"

#include <core/math/anim.h>
#include <core/descriptors/uniform_names.h>
#include <poly/scene_graph.h>
#include <graphics/renderer.h>
#include <render_scene_graph/render_orchestrator.h>

constexpr static uint64_t UI2dMenu = CommandBufferLimits::UI2dStart + 1;

UITapArea::UITapArea( UIView* _owner, const Rect2f& _area, uint64_t _type, UITapAreaStatus _status,
                      std::string _foreground, std::string _background ) :
        owner(_owner), area( _area ), type(_type), status(_status), foreground( std::move(_foreground) ),
        background( std::move(_background)) {
    foregroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "ForeGroundButtonColor" );
    backgroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "BackGroundButtonColor" );
}

void UITapArea::loadResource( uint64_t _idb ) {
    this->owner->SG().load<RawImage>( foreground, [this, _idb](HttpResouceCBSign _key) {
        this->owner->loaded( _idb );
        auto statusColor = owner->colorFromStatus( status );
        backgroundVP = this->owner->RR().drawRect2d( UI2dMenu, area, background, 1.0f, statusColor );
        foregroundVP = this->owner->RR().drawRect2d( UI2dMenu, area, foreground );
    });
}

void UITapArea::transform( float _duration, uint64_t _frameSkipper,
                           const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {

    const float downtime = _duration;
    auto colDown = std::vector<KeyFramePair<V3f>>{
            KeyFramePair{ 0.0f, V3f::ZERO },
            KeyFramePair{ downtime, _pos }
    };

    Timeline::play( backgroundAnim.pos, _frameSkipper, colDown, AnimUpdateCallback([this]() {
        backgroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
        foregroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
    } ));

}

void UITapArea::touchedDown() {
    if ( status != UITapAreaStatus::Enabled ) return;

    const float downtime = 0.1f;
    auto targetColor = checkBitWiseFlag(type, UITapAreaType::stickyButton) ? owner->getSelectedColor() : owner->getPressedDownColor();
    auto colDown = std::vector<KeyFramePair<C4f>>{
            KeyFramePair{ 0.0f, owner->getEnabledColor() },
            KeyFramePair{ downtime, targetColor }
    };

    touchDownAnimNameScale = Timeline::play( backgroundColor, colDown, AnimUpdateCallback([this]() {
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, backgroundColor->value.xyz() );
        backgroundVP->setMaterialConstant( UniformNames::alpha, backgroundColor->value.w() );
    } ));

}

void UITapArea::touchedUp( bool hasBeenTapped, bool isTouchUpGroup ) {

    if ( hasBeenTapped && checkBitWiseFlag(type, UITapAreaType::stickyButton) ) {
        status = UITapAreaStatus::Selected;
    }
    if ( !hasBeenTapped && isTouchUpGroup && checkBitWiseFlag(type, UITapAreaType::stickyButton) ) {
        status = UITapAreaStatus::Enabled;
    }
    setStatus( status );
    auto color = owner->colorFromStatus(status);
    if ( !touchDownAnimNameScale.empty() ) {
        Timeline::stop( backgroundColor, touchDownAnimNameScale, color );
        touchDownAnimNameScale = {};
    }
}

void UITapArea::hoover( bool isHoovering ) {
    if ( status == UITapAreaStatus::Enabled ) {
        auto color = isHoovering ? owner->getHooverColor().xyz() : owner->getEnabledColor().xyz();
        auto alpha = isHoovering ? owner->getHooverColor().w() : owner->getEnabledColor().w();
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color );
        backgroundVP->setMaterialConstant( UniformNames::alpha, alpha );
    }
}

void UITapArea::setStatus( UITapAreaStatus _status ) {
    if ( !ready ) return;
    status = _status;
    auto color = owner->colorFromStatus(status);
    backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color.xyz() );
    backgroundVP->setMaterialConstant( UniformNames::alpha, color.w() );
}

uint64_t UIView::isTapInArea( const V2f& _tap ) const {
    V2f tapS = (_tap / getScreenSizef) * getScreenAspectRatioVector;
    for ( const auto& [k,v] : tapAreas ) {
        if ( v->contains( tapS ) ) {
            return k;
        }
    }
    return 0;
}

std::shared_ptr<UITapArea> UIView::TapArea( uint64_t _key ) const {
    return tapAreas.find(_key)->second;
}

bool UIView::isTouchDownInside( const V2f& _p ) {
    auto k = isTapInArea(_p);
    if ( k ) {
        TapArea(k)->touchedDown();
    }
    return k > 0;
}

void UIView::handleTouchDownUIEvent( const V2f& _p ) {
    touchDownKeyCached( isTapInArea(_p) );
}

void UIView::touchDownKeyCached( uint64_t _key ) const {
    touchDownStartingKey = _key;
//    if ( getButtonStatus(_key) == UITapAreaStatus::Enabled ||
//         getButtonStatus(_key) == UITapAreaStatus::Selected ) {
//        touchDownStartingKey = _key;
//    } else {
//        touchDownStartingKey = 0;
//    }
}

uint64_t UIView::touchDownKeyCached() const {
    return touchDownStartingKey;
}

void UIView::touchedUp( uint64_t _key ) {
    for ( auto& [k, button] : tapAreas ) {
        bool touchUpGroup = false;
        if ( k == 1 || k == 2 || k == 3 ) {
            touchUpGroup = _key == 1 || _key ==2 || _key == 3;
        }
        if ( k ==5 || k == 6 ) {
            touchUpGroup = _key == 5 || _key == 6;
        }
        button->touchedUp( k == _key, touchUpGroup );
    }
}

void UIView::hoover( uint64_t _key) {
    for ( auto& [k, button] : tapAreas ) {
        button->hoover( k == _key );
    }
}

void UIView::addButton( uint64_t _key, const Rect2f& _rect, uint64_t _type, UITapAreaStatus _status,
                        std::string _foreground, std::string _background, const V3f& _initialPos ) {

    if ( tapAreas.empty() ) {
        sg.load<RawImage>( _background, [this](HttpResouceCBSign _key) {
            this->backGroundLoaded = true;
        } );
    }

    auto tap = std::make_shared<UITapArea>(this, _rect, _type, _status, std::move(_foreground), std::move(_background));
    tapAreas.emplace( _key, tap );
    tap->loadResource( _key );
}

void UIView::loaded( uint64_t _key ) {
    tapAreas.at(_key)->loaded();
}

Renderer& UIView::RR() {
    return rsg.RR();
}

SceneGraph& UIView::SG() {
    return sg;
}

C4f UIView::colorFromStatus( UITapAreaStatus _status ) {
    switch (_status) {
        case UITapAreaStatus::Enabled:
            return enabledColor;
        case UITapAreaStatus::Selected:
            return selectedColor;
        case UITapAreaStatus::Disabled:
            return disabledColor;
        case UITapAreaStatus::Hidden:
            return enabledColor;
        case UITapAreaStatus::Hoover:
            return hooverColor;
    }
}

void UIView::setButtonStatus( uint64_t _key, UITapAreaStatus _status ) {
    tapAreas.at(_key)->setStatus( _status );
}

UITapAreaStatus UIView::getButtonStatus( uint64_t _key ) const {
    return tapAreas.at(_key)->status;
}

bool UIView::isButtonEnabled( uint64_t _key ) const {
    auto bs = getButtonStatus( _key );
    return bs == UITapAreaStatus::Enabled || bs == UITapAreaStatus::Selected;
}

void UIView::transform( uint64_t _key, float _duration, uint64_t _frameSkipper,
                        const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {
    tapAreas.at(_key)->transform( _duration, _frameSkipper, _pos, _rot, _scale );
}
