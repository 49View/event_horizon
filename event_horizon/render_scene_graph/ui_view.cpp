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

void UIElement::loadResource( CResourceRef _idb ) {
    this->owner->loaded( _idb );
    auto statusColor = owner->colorFromStatus( status );
    backgroundVP = this->owner->RR().draw<DRect2dRounded>( UI2dMenu, area, statusColor );
    if ( !foreground.empty() ) {
        foregroundVP = this->owner->RR().drawRect2d( UI2dMenu, area, foreground );
    }
}

void UIElement::transform( float _duration, uint64_t _frameSkipper,
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

void UIElement::touchedDown() {
    if ( status != UITapAreaStatus::Enabled ) return;

    const float downtime = 0.1f;
    auto targetColor = checkBitWiseFlag(type(), UIT::stickyButton()) ? owner->getSelectedColor() : owner->getPressedDownColor();
    auto colDown = std::vector<KeyFramePair<C4f>>{
            KeyFramePair{ 0.0f, owner->getEnabledColor() },
            KeyFramePair{ downtime, targetColor }
    };

    touchDownAnimNameScale = Timeline::play( backgroundColor, colDown, AnimUpdateCallback([this](float) {
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, backgroundColor->value.xyz() );
        backgroundVP->setMaterialConstant( UniformNames::alpha, backgroundColor->value.w() );
    } ));

}

void UIElement::touchedUp( bool hasBeenTapped, bool isTouchUpGroup ) {

    if ( hasBeenTapped && checkBitWiseFlag(type(), UIT::stickyButton()) ) {
        status = UITapAreaStatus::Selected;
    }
    if ( !hasBeenTapped && isTouchUpGroup && checkBitWiseFlag(type(), UIT::stickyButton()) ) {
        status = UITapAreaStatus::Enabled;
    }
    setStatus( status );
    auto color = owner->colorFromStatus(status);
    if ( !touchDownAnimNameScale.empty() ) {
        Timeline::stop( backgroundColor, touchDownAnimNameScale, color );
        touchDownAnimNameScale = {};
    }
}

void UIElement::hoover( bool isHoovering ) {
    if ( ready && status == UITapAreaStatus::Enabled ) {
        auto color = isHoovering ? owner->getHooverColor().xyz() : owner->getEnabledColor().xyz();
        auto alpha = isHoovering ? owner->getHooverColor().w() : owner->getEnabledColor().w();
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color );
        backgroundVP->setMaterialConstant( UniformNames::alpha, alpha );
    }
}

void UIElement::setStatus( UITapAreaStatus _status ) {
    if ( !ready ) return;
    status = _status;
    auto color = owner->colorFromStatus(status);
    backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color.xyz() );
    backgroundVP->setMaterialConstant( UniformNames::alpha, color.w() );
}

ResourceRef UIView::isTapInArea( const V2f& _tap ) const {
    V2f tapS = (_tap / getScreenSizef) * getScreenAspectRatioVector;
    for ( const auto& [k,v] : tapAreas ) {
        if ( v->Data().contains( tapS ) ) {
            return k;
        }
    }
    return {};
}

UIElementSP UIView::TapArea( CResourceRef _key ) {
    return tapAreas.find(_key)->second;
}

bool UIView::isTouchDownInside( const V2f& _p ) {
    auto k = isTapInArea(_p);
    if ( !k.empty() ) {
        TapArea(k)->DataRef().touchedDown();
    }
    return !k.empty();
}

void UIView::handleTouchDownUIEvent( const V2f& _p ) {
    touchDownKeyCached( isTapInArea(_p) );
}

void UIView::touchDownKeyCached( CResourceRef _key ) const {
    touchDownStartingKey = _key;
}

CResourceRef UIView::touchDownKeyCached() const {
    return touchDownStartingKey;
}

void UIView::touchedUp( CResourceRef _key ) {
    for ( auto& [k, button] : tapAreas ) {
        bool touchUpGroup = false;
        if ( k == "1" || k == "2" || k == "3" ) {
            touchUpGroup = _key == "1" || _key == "2" || _key == "3";
        }
        if ( k == "5" || k == "6" ) {
            touchUpGroup = _key == "5" || _key == "6";
        }
        button->DataRef().touchedUp( k == _key, touchUpGroup );
    }
}

void UIView::hoover( CResourceRef _key) {
    for ( auto& [k, button] : tapAreas ) {
        button->DataRef().hoover( k == _key );
    }
}

void UIView::loaded( CResourceRef _key ) {
    tapAreas.at(_key)->DataRef().loaded();
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

void UIView::setButtonStatus( CResourceRef _key, UITapAreaStatus _status ) {
    tapAreas.at(_key)->DataRef().setStatus( _status );
}

UITapAreaStatus UIView::getButtonStatus( CResourceRef _key ) const {
    return tapAreas.at(_key)->DataRef().Status();
}

bool UIView::isButtonEnabled( CResourceRef _key ) const {
    auto bs = getButtonStatus( _key );
    return bs == UITapAreaStatus::Enabled || bs == UITapAreaStatus::Selected;
}

void UIView::transform( CResourceRef _key, float _duration, uint64_t _frameSkipper,
                        const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {
    tapAreas.at(_key)->DataRef().transform( _duration, _frameSkipper, _pos, _rot, _scale );
}

void UIView::loadResources() {
    for ( auto& [k,v] : tapAreas ) {
        v->DataRef().loadResource( k );
    }
}

C4f UIView::getEnabledColor() const {
    return colorScheme.Secondary1(0);
}

C4f UIView::getSelectedColor() const {
    return colorScheme.Secondary1(3);
}

C4f UIView::getDisabledColor() const {
    return colorScheme.Secondary1(4);
}

C4f UIView::getHooverColor() const {
    return colorScheme.Secondary1(2);
}

C4f UIView::getPressedDownColor() const {
    return colorScheme.Secondary1(3);
}
