//
// Created by Dado on 2019-05-28.
//

#include "ui_view.hpp"

#include <core/math/anim.h>
#include <core/descriptors/uniform_names.h>
#include <poly/scene_graph.h>
#include <graphics/renderer.h>
#include <render_scene_graph/render_orchestrator.h>

V2f ssOneMinusY( const V2f& _point ) {
    auto ret = (_point / getScreenSizef) * getScreenAspectRatioVector;
    ret.oneMinusY();
    return ret;
}

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
    auto ssBBox = bbox3d->front();
    ssBBox.translate( V2f::Y_AXIS_NEG * ssBBox.height() );

    if ( type() == UIT::separator_h() ) {
        defaultBackgroundColor = C4f::WHITE.A(0.4f);
        backgroundVP = this->owner->RR().draw<DRect2dRounded>(UI2dMenu, ssBBox, _localHierMat,
                RDSRoundedCorner(ssBBox.height()*0.33f), defaultBackgroundColor );
        return;
    }

    if ( text.empty() ) {
        defaultBackgroundColor = type() == UIT::background() ? Color4f::XTORGBA("#036FAB").A(0.9f) : C4f::WHITE.A(0.3f);
        backgroundVP = this->owner->RR().draw<DRect2dRounded>(UI2dMenu, ssBBox, _localHierMat, defaultBackgroundColor);
    }
    if ( !foreground.empty() && text.empty() ) {
        foregroundVP = this->owner->RR().draw<DRect2d>( UI2dMenu, ssBBox, _localHierMat, RDSImage{foreground} );
    }
    if ( !text.empty() ) {
        defaultBackgroundColor = C4f::WHITE;
        this->owner->RR().draw<DText2d>( UI2dMenu,
                FDS{ text, font, ssBBox.bottomLeft(), fontHeight, fontAngle},
                _localHierMat, defaultBackgroundColor );
    }
}

void UIElement::transform( float _duration, uint64_t _frameSkipper,
                           const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {

    const float downtime = _duration;
    auto colDown = std::vector<KeyFramePair<V3f>>{
            KeyFramePair{ 0.0f, backgroundAnim.Pos() },
            KeyFramePair{ downtime, backgroundAnim.Pos() + _pos }
    };

    bbox3d->front().translate( _pos.xy() );

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

    if ( status == UITapAreaStatus::Fixed ) {
        return; // nothing to do here
    }
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
    if ( status == UITapAreaStatus::Enabled ) {
        auto color = isHoovering ? owner->getHooverColor().xyz() : defaultBackgroundColor.xyz();
        auto alpha = isHoovering ? owner->getHooverColor().w() : defaultBackgroundColor.w();
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color );
//        backgroundVP->setMaterialConstant( UniformNames::alpha, alpha );
        backgroundVP->setMaterialConstant( UniformNames::opacity, alpha );
    }
}

void UIElement::setStatus( UITapAreaStatus _status ) {
    status = _status;
    auto color = owner->colorFromStatus(status);
    backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color.xyz() );
//    backgroundVP->setMaterialConstant( UniformNames::alpha, color.w() );
    backgroundVP->setMaterialConstant( UniformNames::opacity, color.w() );
}

bool UIElement::hasActiveStatus() const {
    return !( status == UITapAreaStatus::Fixed || status == UITapAreaStatus::Hidden || status == UITapAreaStatus::Disabled );
}

bool UIElement::contains( const V2f& _point ) const {
    return bbox3dT->containsXY( _point );
}

bool UIElement::containsActive( const V2f& _point ) const {
    if ( !hasActiveStatus() ) return false;
    return bbox3dT->containsXY( _point );
}

void UIElement::singleTap() const {
    if ( singleTapCallback ) singleTapCallback();
}

void UIView::foreach( std::function<void(UIElementSP)> f ) {
    for ( auto& v : elements ) {
        v->foreach( f );
    }
}

void UIView::visit( std::function<void( const UIElementSPConst)> f ) const {
    for ( const auto& v : elements ) {
        v->visit( f );
    }
}

bool UIView::isHandlingUI() const {
    return !activeTaps.empty();
}

bool UIView::pointInUIArea( const V2f& _tap, UICheckActiveOnly _checkFlag ) const {
    bool ret = false;
    V2f tapS = ssOneMinusY(_tap);
    visit( [&]( UIElementSPCC n ) {
        if ( _checkFlag == UICheckActiveOnly::True ? n->Data().containsActive( tapS ) : n->Data().contains( tapS ) ) {
            ret = true;
        }
    } );
    return ret;
}

void UIView::handleTouchDownEvent( const V2f& _p ) {
    V2f tapS = ssOneMinusY(_p);
    activeTaps.clear();
    foreach( [&]( UIElementSP n ) {
        if ( n->Data().containsActive( tapS ) ) {
            activeTaps.emplace_back(n);
        }
    } );
    touchDownKeyCached( activeTaps.empty() ? nullptr : activeTaps.back() );
}

void UIView::handleTouchUpEvent( const V2f& _p ) {

    if ( !activeTaps.empty() ) {
        activeTaps[0]->Data().singleTap();
    }
    activeTaps.clear();
    foreach( [&]( UIElementSP n) {
//        bool touchUpGroup = false;
//        if ( k == "1" || k == "2" || k == "3" ) {
//            touchUpGroup = _key == "1" || _key == "2" || _key == "3";
//        }
//        if ( k == "5" || k == "6" ) {
//            touchUpGroup = _key == "5" || _key == "6";
//        }
//        n->DataRef().touchedUp( _key == n, touchUpGroup );
    });
}

void UIView::touchDownKeyCached( UIElementSP _key ) const {
    touchDownStartingKey = _key;
}

UIElementSP UIView::touchDownKeyCached() const {
    return touchDownStartingKey;
}

void UIView::hoover( const V2f& _point ) {
    V2f tapS = ssOneMinusY(_point);

    foreach( [&]( UIElementSP n) {
        n->DataRef().hoover( n->Data().containsActive( tapS ) );
    });
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
    foreach( [&]( UIElementSP node ) {
        if ( node->Data().Key() == _key ) {
            node->DataRef().setStatus( _status );
        }
    });
}

UITapAreaStatus UIView::getButtonStatus( CResourceRef _key ) const {
    UITapAreaStatus ret = UITapAreaStatus::Enabled;

    visit( [&]( UIElementSPCC node ) {
        if ( node->Data().Key() == _key ) {
            ret = node->Data().Status();
        }
    });

    return ret;
}

bool UIView::isButtonEnabled( CResourceRef _key ) const {
    auto bs = getButtonStatus( _key );
    return bs == UITapAreaStatus::Enabled || bs == UITapAreaStatus::Selected;
}

void UIView::transform( UIElementSP _key, float _duration, uint64_t _frameSkipper,
                        const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {
    _key->DataRef().transform( _duration, _frameSkipper, _pos, _rot, _scale );
}

void UIView::loadResources() {
    foreach( []( UIElementSP node ) {
        node->DataRef().loadResource( node->getLocalHierTransform() );
    });
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
    elements.push_back(_elem);
}

void UIContainer2d::finalise() {
    owner.add( node );

//    node->visit( []( UIElementSP n ) {
//        LOGRS( "BBox: " << *n->BBox3d() );
//        LOGRS( "BBoxT: " << *n->BBox3dT() );
//    } );
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

UIElementSP UIContainer2d::addButton( const ControlDef& _cd, const MScale2d& bsize, CSSDisplayMode displayMode,
        UITapAreaType _bt, const V2f& _pos ) {

    auto child = EF::create<UIElementRT>(PFC{}, _cd, bsize, _bt, UIForegroundIcon{_cd.icon} );
    node->addChildren( child, caret );
    advanceCaret( displayMode, bsize );
    return child;
}

void UIContainer2d::addTitle( const UIFontText& _text ) {
    addLabel( _text, MScale2d{innerPaddedX, _text.height}, CSSDisplayMode::Block );
    addSeparator();
}

void UIContainer2d::addListEntry( const ControlDef& _cd ) {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _cd.textLines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d bsize{ totalTextHeight, totalTextHeight };
    icontrols[_cd.key] = addButton( _cd, bsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _cd.textLines ) {
        auto lsize = MScale2d{innerPaddedX, ltext.height };
        addLabel( ltext, lsize, CSSDisplayMode::Block );
    }
    popCaretX();
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    addSeparator( 0.5f );
}

void UIContainer2d::addListEntryGrid( const ControlDef& _cd, bool _newLine ) {
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _cd.textLines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d bsize{ totalTextHeight, totalTextHeight };
    auto oldCaret = caret;
    icontrols[_cd.key] = addButton( _cd, bsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _cd.textLines ) {
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
                                        const std::vector<ControlDef>& _cds ) {

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    MScale2d bsize{ 0.07f, 0.07f };
    for ( const auto& i : _cds ) {
        icontrols[i.key] = addButton( i, bsize, CSSDisplayMode::Inline, UIT::stickyButton );
    }
    advanceCaret( CSSDisplayMode::Block, MScale2d{0.0f, bsize().y() + padding.y()} );
    for ( size_t t = 0; t < _cds.size(); t++ ) popCaretX();

    for ( const auto& cd : _cds ) {
        for ( const auto& i : cd.textLines ) {
            MScale2d lsize{ bsize().x(), i.height };
            addLabel( i, lsize, CSSDisplayMode::Inline );
        }
    }
    for ( const auto& cd : _cds ) if ( !cd.textLines.empty() ) popCaretX();

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    addSeparator( 0.5f );
}

