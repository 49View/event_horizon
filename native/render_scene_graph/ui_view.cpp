//
// Created by Dado on 2019-05-28.
//

#include "ui_view.hpp"

#include <core/util.h>
#include <core/math/anim.h>
#include <core/descriptors/uniform_names.h>
#include <core/font_utils.hpp>
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


void UIElement::loadResource( std::shared_ptr<Matrix4f> _localHierMat ) {
    auto ssBBox = bbox3d->front();
    ssBBox.translate( V2fc::Y_AXIS_NEG * ssBBox.height() );

    if ( type() == UIT::separator_h() ) {
        defaultBackgroundColor = C4f::WHITE.A(0.4f);
        backgroundVP = rsg.RR().draw<DRect2dRounded>(CommandBufferLimits::UI2dStart, ssBBox, _localHierMat,
                RDSRoundedCorner(ssBBox.height()*0.33f), defaultBackgroundColor );
    } else {
        if ( text.empty() ) {
            defaultBackgroundColor = type() == UIT::background() ? Color4f::XTORGBA("#036FAB").A(0.9f) : C4f::WHITE.A(0.3f);
            backgroundVP =rsg.RR().draw<DRect2dRounded>(CommandBufferLimits::UI2dStart, ssBBox, _localHierMat, defaultBackgroundColor);
        }
        if ( !foreground.empty() && text.empty() ) {
            foregroundVP =rsg.RR().draw<DRect2d>( CommandBufferLimits::UI2dStart, ssBBox, _localHierMat, RDSImage{foreground}, tintColor );
        }
        if ( !text.empty() ) {
            foregroundVP =rsg.RR().draw<DText2d>( CommandBufferLimits::UI2dStart,
                                                  FDS{ text, font, ssBBox.bottomLeft(), fontHeight, fontAngle},
                                                  _localHierMat, fontColor );
        }
    }

    applyVisibility();
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
        if ( backgroundVP ) backgroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
        if ( foregroundVP ) foregroundVP->getTransform()->setTranslation( backgroundAnim.Pos() );
    } ), []() {
    } );

}

void UIElement::touchedDown() {
    if ( status != UIElementStatus::Enabled ) return;

    const float downtime = 0.1f;
    auto targetColor = owner->getPressedDownColor();
    auto colDown = std::vector<KeyFramePair<C4f>>{
            KeyFramePair{ 0.0f, owner->getEnabledColor() },
            KeyFramePair{ downtime, targetColor }
    };

    touchDownAnimNameScale = Timeline::play( backgroundColor, colDown, AnimUpdateCallback([this](float) {
        if ( backgroundVP ) backgroundVP->setMaterialConstant( UniformNames::diffuseColor, backgroundColor->value.xyz() );
        if ( backgroundVP ) backgroundVP->setMaterialConstant( UniformNames::opacity, backgroundColor->value.w() );
    } ));

}

void UIElement::touchedUp( const V2f& _point ) {

    if ( status == UIElementStatus::Fixed ) {
        return; // nothing to do here
    }
    if ( containsActive( _point ) ) {
        if ( checkBitWiseFlag( type(), UIT::stickyButton())) {
            status = UIElementStatus::Selected;
        }
        if ( checkBitWiseFlag( type(), UIT::toggleButton())) {
            status = status == UIElementStatus::Selected ? UIElementStatus::Enabled : UIElementStatus::Selected;
        }
        singleTap();
    } else {
        if ( checkBitWiseFlag( type(), UIT::stickyButton())) {
            toggleSelected();
        }
        status = UIElementStatus::Enabled;
    }

    updateStatus();
}

void UIElement::hoover( bool isHoovering ) {
    if ( status == UIElementStatus::Enabled ) {
        auto color = isHoovering ? owner->getHooverColor().xyz() : defaultBackgroundColor.xyz();
        auto alpha = isHoovering ? owner->getHooverColor().w() : defaultBackgroundColor.w();
        backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color );
        backgroundVP->setMaterialConstant( UniformNames::opacity, alpha );
    }
}

void UIElement::updateStatus() {
    setStatus( status );
    auto color = owner->colorFromStatus(status);
    if ( !touchDownAnimNameScale.empty() ) {
        Timeline::stop( backgroundColor, touchDownAnimNameScale, color );
        touchDownAnimNameScale = {};
    }
    if ( backgroundVP ) backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color.xyz() );
    if ( backgroundVP ) backgroundVP->setMaterialConstant( UniformNames::opacity, color.w() );
}

void UIElement::setStatus( UIElementStatus _status ) {
    status = _status;
    auto color = owner->colorFromStatus(status);
    if ( status == UIElementStatus::Enabled ) color = defaultBackgroundColor;
    backgroundVP->setMaterialConstant( UniformNames::diffuseColor, color.xyz() );
    backgroundVP->setMaterialConstant( UniformNames::opacity, color.w() );
}

bool UIElement::hasActiveStatus() const {
    return !( !bVisible || status == UIElementStatus::Fixed || status == UIElementStatus::Hidden || status == UIElementStatus::Disabled );
}

bool UIElement::contains( const V2f& _point ) const {
    if ( !bVisible ) return false;
    return bbox3dT->containsXY( _point );
}

bool UIElement::containsActive( const V2f& _point ) const {
    if ( !hasActiveStatus() ) return false;
    return bbox3dT->containsXY( _point );
}

void UIElement::singleTap() {
    if ( !bVisible ) return;

    if ( type() == UIT::stickyButton() || type() == UIT::toggleButton() ) {
        if ( status == UIElementStatus::Selected ) {
            singleTapCallback(cbParam);
        } else {
           singleTapOffToggleCallback(cbParam);
        }
        toggleSelected();
    } else {
        singleTapCallback(cbParam);
    }
}

void UIElement::toggleSelected() {
    for ( auto& groupElem : groupElements ) {
        if ( groupElem->DataRef().Status() == UIElementStatus::Selected ) {
            groupElem->DataRef().toggle();
        }
    }
}

void UIElement::toggle() {
    if ( type() == UIT::stickyButton() || type() == UIT::toggleButton() ) {
        if ( status == UIElementStatus::Selected ) {
            status = UIElementStatus::Enabled;
            singleTapOffToggleCallback(0);
        } else {
            singleTapCallback(0);
            status = UIElementStatus::Selected;
        }
        updateStatus();
    }
}

void UIElement::applyVisibility() {
    if ( foregroundVP ) foregroundVP->setHidden( !bVisible );
    if ( backgroundVP ) backgroundVP->setHidden( !bVisible );
}

void UIElement::setVisible( bool _value ) {
    bVisible = _value;
    applyVisibility();
}

void UIElement::fadeTo( float _duration, float _value ) {
    fader( _duration, _value, backgroundVP, foregroundVP );
    Timeline::intermezzo( _duration, 1, AnimUpdateCallback([this, _value,_duration](float _elapsed) {
        if ( _elapsed == 0.0f && _value > 0.0f ) {
            setVisible( true );
        } else if ( _elapsed >= _duration && _value == 0.0f ) {
            setVisible( false );
        }
    }));
}

void UIElement::insertGroupElement( UIElementSP _elem ) {
    groupElements.emplace_back( _elem );
}

void UIView::foreach( std::function<void(UIElementSP)> f ) {
    for ( auto& [k,v] : elements ) {
        v->foreach( f );
    }
}

void UIView::visit( std::function<void( const UIElementSPConst)> f ) const {
    for ( const auto& [k,v] : elements ) {
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
            n->DataRef().touchedDown();
            n->moveDownRight(0.03f, MScale2dXS{0.0005f}());
        }
    } );
    touchDownKeyCached( activeTaps.empty() ? nullptr : activeTaps.back() );
}

void UIView::clearOnTouchUpEvent() {

//    foreach( [&]( UIElementSP n) {
//        if ( checkBitWiseFlag( n->DataRef().Type(), UIT::stickyButton()) ) {
//            n->DataRef().toggle();
//        }
//    });

    activeTaps.clear();
}

void UIView::handleTouchUpEvent( const V2f& _p ) {

    V2f tapS = ssOneMinusY(_p);
    for ( auto& tap : activeTaps ) {
        tap->DataRef().touchedUp( tapS );
        tap->moveUpLeft( 0.03f, 0.0f );
    }
    clearOnTouchUpEvent();
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

C4f UIView::colorFromStatus( UIElementStatus _status ) {
    switch (_status) {
        case UIElementStatus::Enabled:
            return getEnabledColor();
        case UIElementStatus::Selected:
            return getSelectedColor();
        case UIElementStatus::Disabled:
            return getDisabledColor();
        case UIElementStatus::Hidden:
            return getEnabledColor();
        case UIElementStatus::Hoover:
            return getHooverColor();
        case UIElementStatus::Fixed:
            return colorScheme.Complement(1);
        default:
            return C4f::WHITE;
    }
}

void UIView::setButtonStatus( CResourceRef _key, UIElementStatus _status ) {
    foreach( [&]( UIElementSP node ) {
        if ( node->Data().Key() == _key ) {
            node->DataRef().setStatus( _status );
        }
    });
}

UIElementStatus UIView::getButtonStatus( CResourceRef _key ) const {
    UIElementStatus ret = UIElementStatus::Enabled;

    visit( [&]( UIElementSPCC node ) {
        if ( node->Data().Key() == _key ) {
            ret = node->Data().Status();
        }
    });

    return ret;
}

bool UIView::isButtonEnabled( CResourceRef _key ) const {
    auto bs = getButtonStatus( _key );
    return bs == UIElementStatus::Enabled || bs == UIElementStatus::Selected;
}

void UIView::loadResources() {
    foreach( []( UIElementSP node ) {
        node->DataRef().loadResource( node->getLocalHierTransform() );
    });
}

void UIView::updateAnim() {
    foreach( []( UIElementSP node ) {
        node->updateAnim();
    });
}

C4f UIView::getEnabledColor() const {
    return C4f::WHITE.A(0.4f);//colorScheme.Secondary1(0);
}

C4f UIView::getSelectedColor() const {
    return C4f::DARK_GRAY.A(0.5f);//colorScheme.Secondary1(3);
}

C4f UIView::getDisabledColor() const {
    return C4f::DARK_GRAY.A(0.15f); //colorScheme.Secondary1(4);
}

C4f UIView::getHooverColor() const {
    return colorScheme.Secondary1(2);
}

C4f UIView::getPressedDownColor() const {
    return colorScheme.Secondary1(2).A(0.65f);
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

void UIView::add( UIElementSP _elem, UIElementStatus _initialStatus ) {
    addRecursive( _elem );
    _elem->setVisible( _initialStatus != UIElementStatus::Hidden );
    elements.emplace( _elem->Name(), _elem);
}

void UIView::add( const MPos2d& _at, UIViewContainer& _container, UIElementStatus _initialStatus ) {
    _container.finalize(_at);
    add( _container.Node(), _initialStatus );
}

UIElementSP UIView::node( CResourceRef& _key ) {
    return elements[_key];
}

UIElementSP UIView::operator()( const std::string& _key ) {
    return node(_key);
}

void UIView::addCallback( const std::string& _key, UICallbackFunc cf ) {
    callbacks.emplace( _key, cf );
}

UICallbackMap& UIView::Callbacks() {
    return callbacks;
}

void UIView::clear() {
    elements.clear();
    callbacks.clear();
    activeTaps.clear();
}

void UIViewContainer::advanceCaret( CSSDisplayMode _displayMode, const MScale2d& _elemSize ) {

    if ( _displayMode == CSSDisplayMode::Block ) {
        caret -= V2f{ 0.0f, _elemSize().y() - padding.y()};
        float elemS = _elemSize().x() == 0.0f ? 0.0f : _elemSize().x() + padding.x();
        boundaries = max(boundaries, { caret.x() + elemS, -caret.y()} );
    }
    if ( _displayMode == CSSDisplayMode::Inline ) {
        caretQueue.push_back( caret );
        caret += V2f{ padding.x() + _elemSize().x(), 0.0f };
        boundaries = max(boundaries, { caret.x() - padding.x(), -caret.y() + _elemSize().y()} );
    }

}

void UIViewContainer::addEmptyCaret() {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
}

void UIViewContainer::addEmptyCaretNewLine() {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    caret.setX( padding.x() );
}

void UIViewContainer::popCaretX() {
    auto lc = caretQueue.back();
    caret.setX( lc.x() );
    caretQueue.pop_back();
}

void UIViewContainer::addSeparator( float percScaleY ) {
    auto childDadeT2 = EF::create<UIElementRT>(PFC{}, rsg, UUIDGen::make(), MScale2d{innerPaddedX, 0.0025f*percScaleY},
                                               UIT::separator_h );

    wholeLiners.push_back( node->addChildren( childDadeT2, caret ) );
    advanceCaret( CSSDisplayMode::Block, MScale2d{ innerPaddedX, padding.y() } );
}

void UIViewContainer::addLabel( const UIFontText& _text,
                                const MScale2d& lsize, CSSDisplayMode displayMode, const V2f& _pos ) {
    auto fsize = FontUtils::measure( _text.text, rsg.SG().get<Font>(_text.fontRef).get(), _text.height ).size();
    fsize.setY( _text.height );
    auto child = EF::create<UIElementRT>(PFC{}, rsg, UUIDGen::make(), UIT::label, MScale2d{fsize}, _text );
//    child->BBox3d( V2fc::ZERO, fsize );
    auto tpos = caret;
    if ( lsize().x() != 0.0f ) tpos.setX( (lsize().x() - fsize.x()*0.5f) );
    node->addChildren( child, tpos );
    advanceCaret( displayMode, MScale2d{fsize} );
}

UIElementSP UIViewContainer::addButton( const ControlDef& _cd, const MScale2d& _bsize, CSSDisplayMode displayMode,
                                        UITapAreaType _bt, const V2f& _pos ) {

    auto child = EF::create<UIElementRT>(PFC{}, rsg, _cd, _bsize, _bt, UIForegroundIcon{_cd.icon}, _cd.tintColor, _cd.cbParam );
    node->addChildren( child, caret );
    advanceCaret( displayMode, _bsize );
    return child;
}

void UIViewContainer::addTitle( const UIFontText& _text, UIAlignFlags _flags ) {
    addLabel( _text, MScale2d{innerPaddedX, _text.height}, CSSDisplayMode::Block );
    if ( !checkBitWiseFlag(_flags, UIAF_DoNotAddSeparator ) ) {
        addSeparator();
    }
}

void UIViewContainer::addNavBar( const ControlDef& _logo ) {
//    addButton( _logo, MScale2d{0.06f, 0.06f}, CSSDisplayMode::Inline );

//    auto padding = 0.01f * getScreenAspectRatio;
//    auto headerTop = 0.94f;
//    rsg.RR().drawRect2d( CommandBufferLimits::UI2dStart,
//                         V2f{ padding, headerTop },
//                         V2f{ getScreenAspectRatioVector.x(), 1.0f - headerTop } -
//                         V2f{ padding * 2.0f, padding / getScreenAspectRatio },
//                         C4f::PASTEL_YELLOW.A( 0.9f ));
//
//    float ar = sg.TL( "carillo,logo" )->getAspectRatio();
//    float logoPadding = (( 1.0f - headerTop ) - padding - 0.01f ) * 0.5f;
//    rsg.RR().drawRect2d( CommandBufferLimits::UI2dStart, V2f{ 0.03f, headerTop + logoPadding },
//                         V2f{ 0.03f + ar * 0.02f, headerTop + logoPadding + 0.02f },
//                         "carillo,logo" );
}

void UIViewContainer::addListEntry( const ControlDef& _cd ) {
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _cd.textLines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d lbsize{ totalTextHeight, totalTextHeight };
    icontrols[_cd.key] = addButton( _cd, lbsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _cd.textLines ) {
        auto lsize = MScale2d{innerPaddedX, ltext.height };
        addLabel( ltext, lsize, CSSDisplayMode::Block );
    }
    popCaretX();
    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    addSeparator( 0.5f );
}

void UIViewContainer::addListEntryGrid( const ControlDef& _cd, bool _newLine, bool _lastOne ) {
    float totalTextHeight = 0.0f;
    for ( const auto& ltext : _cd.textLines ) totalTextHeight += ltext.height + (-padding.y());
    MScale2d lbsize{ totalTextHeight, totalTextHeight };
    auto oldCaret = caret;
    icontrols[_cd.key] = addButton( _cd, lbsize, CSSDisplayMode::Inline );
    for ( const auto& ltext : _cd.textLines ) {
        auto lsize = MScale2d{innerPaddedX, ltext.height };
        addLabel( ltext, lsize, CSSDisplayMode::Block );
    }
    caret = oldCaret;
    if ( !_newLine && !_lastOne ) {
        advanceCaret( CSSDisplayMode::Inline, MScale2d{0.30f, 0.0f} );
    } else {
        caret.setX( padding.x() );
        advanceCaret( CSSDisplayMode::Block, MScale2d{0.0f, totalTextHeight} );
        addEmptyCaret();
    }
}

void UIViewContainer::addButtonGroupLine( UITapAreaType _uit,
                                          const std::vector<ControlDef>& _cds, UIAlignFlags _flags ) {

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    for ( const auto& i : _cds ) {
        icontrols[i.key] = addButton( i, bsize, CSSDisplayMode::Inline, _uit );
    }
    if ( _uit() == UIT::stickyButton() || _uit() == UIT::toggleButton() ) {
        for ( size_t t = 0; t < _cds.size(); t++ ) {
            for ( size_t m = 0; m < _cds.size(); m++ ) {
                if ( m != t ) {
                    icontrols[_cds[m].key]->DataRef().insertGroupElement(icontrols[_cds[t].key]);
                }
            }
        }
    }

    advanceCaret( CSSDisplayMode::Block, MScale2d{0.0f, bsize().y() + padding.y()} );
    for ( size_t t = 0; t < _cds.size(); t++ ) popCaretX();

    float xt = padding.x();
    for ( const auto& cd : _cds ) {
        for ( const auto& i : cd.textLines ) {
            MScale2d lsize{ xt + bsize().x()*0.5f, i.height };
            addLabel( i, lsize, CSSDisplayMode::Inline );
        }
        xt += padding.x() + bsize().x();
    }
    for ( const auto& cd : _cds ) if ( !cd.textLines.empty() ) popCaretX();

    advanceCaret( CSSDisplayMode::Block, wholeLineSize );
    if ( !checkBitWiseFlag(_flags, UIAF_DoNotAddSeparator ) ) {
        addSeparator( 0.5f );
    }
}

void UIViewContainer::setButtonSize( const MScale2d& _bs ) {
    bsize = _bs;
}

void UIViewContainer::setPadding( const V2f& _value ) {
    padding = _value;
    caret = padding;
}

V3f UIViewContainer::getSize() const {
    return size;
}

void UIViewContainer::finalize( const MPos2d& _at ) {
    auto fakeAA = AABB::MIDENTITY();
    fakeAA.scaleX(boundaries.x());
    fakeAA.scaleY(boundaries.y() );
    innerPaddedX = boundaries.x() - ( padding.x() * 2.0f );
    for ( auto& wle : wholeLiners ) {
        wle->DataRef().BBox3d()->setMaxPoint(V3f{innerPaddedX, wle->DataRef().BBox3d()->maxPoint().y(), 0.0f} );
    }
    Node()->DataRef().BBox3d( fakeAA );
    Node()->updateTransform( _at() );
}

void UIViewContainer::raii( const std::string& _name ) {
    node = EF::create<UIElementRT>( PFC{}, rsg, UUIDGen::make(), pos, UIT::background );
    node->Name( _name );
    caret = padding;
    wholeLineSize = MScale2d{ innerPaddedX, -padding.y() };
}

UITapAreaType tapTypeFromString( const std::string& _value ) {
    if ( _value == "background" )       return UIT::background;
    if ( _value == "pushButton" )       return UIT::pushButton;
    if ( _value == "stickyButton"  )    return UIT::stickyButton;
    if ( _value == "label" )            return UIT::label;
    if ( _value == "separator" )        return UIT::separator_h;
    if ( _value == "separator_v" )      return UIT::separator_v;
    if ( _value == "toggleButton" )     return UIT::toggleButton;
    return UIT::background;
}

void UIViewContainer::unpack( UIContainer* _data ) {
    if ( !_data ) return;

    std::unordered_map<std::string, float> uiFontSizes{
            { "title", 0.025f }, { "lead", 0.020f }, { "normal", 0.016f }
    };
    for ( auto entry : _data->entries ) {
        entry.fixUpDefaults();
        if ( entry.type == "EmptyCaret" ) {
            addEmptyCaret();
        }
        if ( entry.type == "Title" ) {
            addTitle( { entry.font, uiFontSizes["title"], entry.text } );
        } else if ( entry.type == "ListEntry" ) {
            std::vector<UIFontText> te;
            for ( auto tf : entry.entries ) {
                tf.fixUpDefaults();
                te.emplace_back( tf.font, uiFontSizes[tf.size], C4f::XTORGBA(tf.color), tf.text );
            }
            auto cb = entry.func.empty() ? sUIEmptyCallback : rsg.UICB()[entry.func[0]];
            auto cbParam = entry.func.size() >= 2 ? entry.func[1] : "-1";
            addListEntry( { entry.id, entry.icon, te, cb, cbParam  } );
        } else if ( entry.type == "ButtonGroupLine" ) {
            std::vector<ControlDef> te;
            for ( auto tf : entry.entries ) {
                tf.fixUpDefaults();
                auto cb = tf.func.empty() ? sUIEmptyCallback : rsg.UICB()[tf.func[0]];
                auto cbParam = tf.func.size() >= 2 ? tf.func[1] : "-1";
                te.emplace_back( ControlDef{tf.id, tf.icon, {tf.font, uiFontSizes[tf.size], C4f::XTORGBA(tf.color), tf.text}, cb, cbParam} );
            }

            addButtonGroupLine( tapTypeFromString(entry.tapType), te );
        }
    }
//    LOGRS( cl.type );
}
