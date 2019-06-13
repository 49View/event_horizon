//
// Created by Dado on 2019-05-28.
//

#include "ui_view.hpp"

#include <core/math/anim.h>
#include <core/descriptors/uniform_names.h>
#include <poly/scene_graph.h>
#include <graphics/renderer.h>
#include <render_scene_graph/render_orchestrator.h>

ColorScheme::ColorScheme( const std::string& colorDescriptor ) {
    size_t i0 = 0;
    for ( size_t t = 0; t < 4; t++ ) {
        for ( size_t q = 0; q < 5; q++ ) {
            auto i1 = colorDescriptor.find("shade " + std::to_string(q), i0);
            auto i2 = colorDescriptor.find('#', i1);
            auto pc = colorDescriptor.substr( i2, 7);
            switch ( t ) {
                case 0:
                    primaryColors[q] = C4f::XTORGBA( pc);
                    break;
                case 1:
                    secondary1Colors[q] = C4f::XTORGBA( pc);
                    break;
                case 2:
                    secondary2Colors[q] = C4f::XTORGBA( pc);
                    break;
                case 3:
                    complementColors[q] = C4f::XTORGBA( pc);
                    break;
                default:
                    break;
            }
            i0 = i2;
        }
    }
}

constexpr static uint64_t UI2dMenu = CommandBufferLimits::UI2dStart + 1;

UITapArea::UITapArea( UIView* _owner, const Rect2f& _area, uint64_t _type, UITapAreaStatus _status,
                      std::string _foreground, std::string _background ) :
        owner(_owner), area( _area ), type(_type), status(_status), foreground( std::move(_foreground) ),
        background( std::move(_background)) {
    foregroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "ForeGroundButtonColor" );
    backgroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "BackGroundButtonColor" );
}

void UITapArea::loadResource( uint64_t _idb ) {
    this->owner->loaded( _idb );
    auto statusColor = owner->colorFromStatus( status );
    backgroundVP = this->owner->RR().drawRect2d( UI2dMenu, area, background, 1.0f, statusColor );
    foregroundVP = this->owner->RR().drawRect2d( UI2dMenu, area, foreground );
}

void UITapArea::transform( float _duration, uint64_t _frameSkipper,
                           const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {

    const float downtime = _duration;
    auto colDown = std::vector<KeyFramePair<V3f>>{
            KeyFramePair{ 0.0f, backgroundAnim.Pos() },
            KeyFramePair{ downtime, backgroundAnim.Pos() + _pos }
    };

    area.translate( _pos.xy() );

    Timeline::play( backgroundAnim.pos, _frameSkipper, colDown, AnimUpdateCallback([this](float) {
        backgroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
        foregroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
    } ), []() {
    } );

}

void UITapArea::touchedDown() {
    if ( status != UITapAreaStatus::Enabled ) return;

    const float downtime = 0.1f;
    auto targetColor = checkBitWiseFlag(type, UITapAreaType::stickyButton) ? owner->getSelectedColor() : owner->getPressedDownColor();
    auto colDown = std::vector<KeyFramePair<C4f>>{
            KeyFramePair{ 0.0f, owner->getEnabledColor() },
            KeyFramePair{ downtime, targetColor }
    };

    touchDownAnimNameScale = Timeline::play( backgroundColor, colDown, AnimUpdateCallback([this](float) {
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
    if ( ready && status == UITapAreaStatus::Enabled ) {
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

    tapAreas.emplace( _key, std::make_shared<UITapArea>(this, _rect, _type, _status, std::move(_foreground), std::move(_background)) );
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
            return getEnabledColor();
        case UITapAreaStatus::Selected:
            return getSelectedColor();
        case UITapAreaStatus::Disabled:
            return getDisabledColor();
        case UITapAreaStatus::Hidden:
            return getEnabledColor();
        case UITapAreaStatus::Hoover:
            return getHooverColor();
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

void UIView::loadResources() {
    for ( auto& [k,v] : tapAreas ) {
        v->loadResource( k );
    }
}
