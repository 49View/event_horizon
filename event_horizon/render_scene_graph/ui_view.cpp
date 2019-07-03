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

void UIElement::loadResource( std::shared_ptr<Matrix4f> _localHierMat ) {
//    auto statusColor = owner->colorFromStatus( status );
    auto ssBBox = bbox3d->topDown();
    ssBBox.translate( V2f::Y_AXIS_NEG * ssBBox.height() );

    if ( type() == UIT::separator_h() ) {
        backgroundVP = this->owner->RR().draw<DRect2dRounded>(UI2dMenu, ssBBox, _localHierMat,
                RDSRoundedCorner(ssBBox.height()*0.33f), C4f::WHITE.A(0.45f));
        return;
    }

    if ( text.empty() ) {
        backgroundVP = this->owner->RR().draw<DRect2dRounded>(UI2dMenu, ssBBox, _localHierMat, C4f::WHITE.A(0.3f));
    }
    if ( !foreground.empty() && text.empty() ) {
        foregroundVP = this->owner->RR().draw<DRect2d>( UI2dMenu, ssBBox, _localHierMat, RDSImage{foreground} );
    }
    if ( !text.empty() ) {
        this->owner->RR().draw<DText2d>( UI2dMenu,
                FDS{ text, font, ssBBox.bottomLeft(), fontHeight, fontAngle},
                _localHierMat, C4f::WHITE );
    }
}

void UIElement::transform( float _duration, uint64_t _frameSkipper,
                           const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {

    const float downtime = _duration;
    auto colDown = std::vector<KeyFramePair<V3f>>{
            KeyFramePair{ 0.0f, backgroundAnim.Pos() },
            KeyFramePair{ downtime, backgroundAnim.Pos() + _pos }
    };

    bbox3d->topDown().translate( _pos.xy() );

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

bool UIElement::contains( const V2f& _point ) const {
    return bbox3dT->containsXZ( _point );
}

ResourceRef UIView::isTapInArea( const V2f& _tap ) const {
    V2f tapS = (_tap / getScreenSizef) * getScreenAspectRatioVector;
    for ( const auto& [k,v] : elements ) {
        if ( v->Data().contains( tapS ) ) {
            return k;
        }
    }
    return {};
}

UIElementSP UIView::TapArea( CResourceRef _key ) {
    return elements.find( _key)->second;
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
    for ( auto& [k, button] : elements ) {
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
    for ( auto& [k, button] : elements ) {
        button->DataRef().hoover( k == _key );
    }
}

void UIView::loaded( CResourceRef _key ) {
    elements.at( _key)->DataRef().loaded();
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
        case UITapAreaStatus::Fixed:
            return colorScheme.Complement(1);
    }
}

void UIView::setButtonStatus( CResourceRef _key, UITapAreaStatus _status ) {
    elements.at( _key)->DataRef().setStatus( _status );
}

UITapAreaStatus UIView::getButtonStatus( CResourceRef _key ) const {
    return elements.at( _key)->DataRef().Status();
}

bool UIView::isButtonEnabled( CResourceRef _key ) const {
    auto bs = getButtonStatus( _key );
    return bs == UITapAreaStatus::Enabled || bs == UITapAreaStatus::Selected;
}

void UIView::transform( CResourceRef _key, float _duration, uint64_t _frameSkipper,
                        const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {
    elements.at( _key)->DataRef().transform( _duration, _frameSkipper, _pos, _rot, _scale );
}

void UIView::loadResources() {
    for ( auto& [k,v] : elements ) {
        v->visit( []( UIElementSP node) {
            node->DataRef().loadResource( node->getLocalHierTransform() );
        });
//        loadResourcesRec( v );
        loaded( k );
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

void UIView::addRecursive( UIElementSP _elem ) {
    _elem->DataRef().Owner(this);
    if ( !_elem->DataRef().FontRef().empty() ) {
        _elem->DataRef().Font( sg.FM().get(S::DEFAULT_FONT).get() );
    }

    for ( const auto& c : _elem->Children() ) {
        addRecursive( c );
    }
}

void UIView::add( UIElementSP _elem ) {
    addRecursive( _elem );
    elements[_elem->Data().Key()] = _elem;
}

void UIContainer2d::finalise() {
    owner.add( node );
}

void UIContainer2d::advanceCaret( CSSDisplayMode _displayMode, const MScale2d& _elemSize ) {

    if ( _displayMode == CSSDisplayMode::Block ) {
        caret -= V2f{ 0.0f, _elemSize().y() - padding.y()};
    }
    if ( _displayMode == CSSDisplayMode::Inline ) {
        caretQueue.push_back( caret );
        caret += V2f{ padding.x() + _elemSize().x(), 0.0f };
    }

}

void UIContainer2d::addEmptyCaret() {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
}

void UIContainer2d::addEmptyCaretNewLine() {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    caret.setX( padding.x() );
}

void UIContainer2d::popCaretX() {
    auto lc = caretQueue.back();
    caret.setX( lc.x() );
    caretQueue.pop_back();
}

void UIContainer2d::addSeparator( float percScaleY ) {
    auto childDadeT2 = EF::create<UIElementRT>(PFC{}, UUIDGen::make(), MScale2d{innerPaddedX, 0.0025f*percScaleY},
                                               UIT::separator_h );

    node->addChildren( childDadeT2, caret );
    advanceCaret( CSSDisplayMode::Block, MScale2d{ innerPaddedX, padding.y() } );
}

void UIContainer2d::addLabel( const UIFontText& _text,
                              const MScale2d& lsize, CSSDisplayMode displayMode, const V2f& _pos ) {
    auto child = EF::create<UIElementRT>(PFC{}, UUIDGen::make(), UIT::label, lsize, _text );
    node->addChildren( child, caret );
    advanceCaret( displayMode, lsize );
}

void UIContainer2d::addButton( CResourceRef _icon, const MScale2d& bsize, CSSDisplayMode displayMode,
        UITapAreaType _bt, const V2f& _pos ) {

    auto child = EF::create<UIElementRT>(PFC{}, UUIDGen::make(), bsize, _bt, UIForegroundIcon{_icon} );
    node->addChildren( child, caret );
    advanceCaret( displayMode, bsize );
}

void UIContainer2d::addTitle( const UIFontText& _text ) {
    addLabel( _text, MScale2d{innerPaddedX, _text.height}, CSSDisplayMode::Block );
    addSeparator();
}

void UIContainer2d::addListEntry( CResourceRef _icon, const std::vector<UIFontText>& _lines ) {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _lines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d bsize{ totalTextHeight, totalTextHeight };
    addButton( _icon, bsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _lines ) {
        auto lsize = MScale2d{innerPaddedX, ltext.height };
        addLabel( ltext, lsize, CSSDisplayMode::Block );
    }
    popCaretX();
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    addSeparator( 0.5f );
}

void UIContainer2d::addListEntryGrid( CResourceRef _icon, const std::vector<UIFontText>& _lines, bool _newLine ) {
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _lines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d bsize{ totalTextHeight, totalTextHeight };
    auto oldCaret = caret;
    addButton( _icon, bsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _lines ) {
        auto lsize = MScale2d{innerPaddedX, ltext.height };
        addLabel( ltext, lsize, CSSDisplayMode::Block );
    }
    caret = oldCaret;
    if ( !_newLine ) {
        advanceCaret( CSSDisplayMode::Inline, MScale2d{0.30f, 0.0f} );
    } else {
        caret.setX( padding.x() );
        advanceCaret( CSSDisplayMode::Block, MScale2d{0.0f, totalTextHeight} );
        addEmptyCaret();
    }
}

void UIContainer2d::addButtonGroupLine( UITapAreaType _uit,
                                        const std::vector<ResourceRef>& _icons,
                                        const std::vector<UIFontText>& _labels ) {

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    MScale2d bsize{ 0.07f, 0.07f };
    for ( const auto& i : _icons ) {
        addButton( i, bsize, CSSDisplayMode::Inline, UIT::stickyButton );
    }
    advanceCaret( CSSDisplayMode::Block, MScale2d{0.0f, bsize().y() + padding.y()} );
    for ( size_t t = 0; t < _icons.size(); t++ ) popCaretX();

    if ( !_labels.empty() ) {
        for ( const auto& i : _labels ) {
            MScale2d lsize{ bsize().x(), i.height };
            addLabel( i, lsize, CSSDisplayMode::Inline );
        }
        for ( size_t t = 0; t < _icons.size(); t++ ) popCaretX();
    }

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    addSeparator( 0.5f );
}

