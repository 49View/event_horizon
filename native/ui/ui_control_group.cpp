#include "ui_control_group.h"

#include "core/command.hpp"
#include "ui_bsdata.hpp"
#include "ui_background.h"
#include "ui_label.h"
#include "ui_image.h"
#include "ui_button.h"
#include "ui_checkbox.h"
#include "ui_slider.h"
#include "ui_input_box.h"
#include "ui_separator.h"

#include "ui_control_manager.hpp"

UiControlGroup::UiControlGroup( UiControlManager& _uicm, const std::string& _uiViewName, int _flags,
                                const KVFMapping& _mapping ) : uicm( _uicm ) {

    static int mNextId{};
    mHash = ++mNextId;
    mVisible = true;
    mAnimMatrix = MatrixAnim();
    mPreRotation = Matrix4f::IDENTITY;

    mMapping = _mapping;
    mSchemaName = _uiViewName;
    mUIZ = uicm.ZLevelStack();
    mUIZ += 2;

    const UIView& view = uicm.getView( mSchemaName );

    mStartPosition = getStartingOffsetFromRootPosition( view.rootPosition, view.customRootPosition );
    mMargins = view.margins;
    mOffsets = mStartPosition;
    mFlags = _flags;
    IsScreenSpace( checkBitWiseFlag( mFlags, UiControlFlag::UseScreenSpace ));

    for ( const auto& arrangement : view.arrangements ) {
        push_layout( arrangement );
    }

    uicm.pushZLevelStack( mUIZ );
}

Vector2f UiControlGroup::getStartingOffsetFromRootPosition( const std::string& _rootPos,
                                                            const Vector2f& _customTRootPos ) {

    if ( _rootPos == "Custom" ) {
        return _customTRootPos;
    } else if ( _rootPos == "TopLeft" ) {
        return Vector2f::Y_AXIS;
    }

    return Vector2f::ZERO;
}

void UiControlGroup::pushAnchor( const Vector2f& _size, const UiAlignElement& _margins ) {
    mOffsets += _margins.TopLeftI();
    mAnchors.push( { mOffsets, _size.x0() } );
}

void UiControlGroup::popAnchor( const std::string& _anchorPos, const UiAlignElement& _margins ) {
    auto npos = UiControl::stringToUiControlPosition(_anchorPos);
    Vector2f pAnchor = mAnchors.top().xy();
    if ( npos == UiControlPosition::ContinueX ) {
        mOffsets = pAnchor + Vector2f{ mAnchors.top().z(), 0.0f };
    }
    if ( npos == UiControlPosition::ContinueY ) {
        mOffsets = Vector2f{ pAnchor.x() - mMargins.Left(), mOffsets.y() - _margins.Bottom() };
    }
    mAnchors.pop();
}

ControlGroupAlignData
UiControlGroup::pushAlignment( const UiControlPosition npos, const UiControlSizeType sizeType,
                               Vector2f& size,
                               const int flags, const UiAlignElement _margins, const UiAlignElement _padding ) {
    UiAlignElement margins = _margins;//.isValid() ? _margins : l.margins;
    UiAlignElement padding = _padding;//.isValid() ? _padding : l.padding;

    bool bFloatingRight = checkBitWiseFlag( flags, UiControlFlag::FloatRight );

    Vector2f realSize = size;

    if ( sizeType == UiControlSizeType::FullSize ) {
        realSize = fullSizeWithMargins( margins, padding );
    }
    if ( sizeType == UiControlSizeType::FullQuad ) {
        realSize = fullSizeQuadWithMargins( margins, padding );
    }
    if ( sizeType == UiControlSizeType::FullWidth ) {
        realSize = fullWidthWithMargins( size.y(), margins, padding );
    }
    if ( bFloatingRight ) {
        size = remainingWidthWithMargins( size.y(), margins, padding );
        realSize = size;
    }

    Vector2f lOrigin = Vector2f( mOffsets.x() + margins.Left() + padding.Left(),
                                 ( mOffsets.y() - ( margins.Top() + padding.Top())));

    if ( npos == UiControlPosition::Overlap ) {
        lOrigin = mAnchors.top().xy();
    }

    if ( npos == UiControlPosition::ContinueX ) {
        mOffsets += Vector2f{ realSize.x() + margins.Width() + padding.Width(), 0.0f };
    }
    if ( npos == UiControlPosition::ContinueY || bFloatingRight ) {
        mOffsets += Vector2f{ 0.0f, -( realSize.y() + margins.Height() + padding.Height() ) };
    }

    //LOGI( "lOrigin: [%f, %f]", lOrigin.x(), lOrigin.y() );
    //LOGI( "mOffset: [%f, %f]", mOffsets.x(), mOffsets.y() );

    return { lOrigin, realSize };
}

void UiControlGroup::overrideStyleIfNeeded( UiControlInitData& cid, const StyleSheetElement& ss ) const {
    if ( ss.sizeType != "" ) {
        cid.sizeType = UiControl::stringToUiControlSizeType( ss.sizeType );
    }
    if ( ss.text != "" ) {
        cid.text = ss.text;
    }
    if ( ss.size.x() >= 0.0f ) {
        cid.size = ss.size;
    }
    if ( ss.color.x() >= 0.0f ) {
        cid.color = Color4f::FTORGBA( ss.color );
    }
    if ( ss.margins.x() >= 0.0f ) {
        cid.margins = ss.margins;
    }
    if ( ss.padding.x() >= 0.0f ) {
        cid.padding = ss.padding;
    }
    if ( !ss.flags.empty()) {
        for ( auto& f : ss.flags ) {
            cid.flags |= UiControl::stringToUiControlFlag( f );
        }
    }
}

int UiControlGroup::getNumberOfArrayLayoutsFromInjection( const Layout& l, const LayoutArrangement& _layoutArrangement ) {

    if ( !_layoutArrangement.is_array ) return 1;

    int maxArrayInjectionElements = 0;

    for ( auto& element : l.elements ) {
        std::string mkey = element.text + _layoutArrangement.style;
        auto string_inj = mMapping.findValue( mkey + std::to_string( 0 ));
        int lc = 0;
        while ( !string_inj.empty()) {
            string_inj = mMapping.findValue( mkey + std::to_string( ++lc ));
        }
        if ( lc > maxArrayInjectionElements ) maxArrayInjectionElements = lc;
    }

    if ( maxArrayInjectionElements == 0 ) return 1;

    return maxArrayInjectionElements;
}

std::string getLayoutName( const Layout& /*_layout*/, int _index, const LayoutArrangement& _layoutArrangement ) {
    std::string layoutNameExpanded = _layoutArrangement.layout.name;
    if ( !_layoutArrangement.style.empty()) layoutNameExpanded += _layoutArrangement.style;
    layoutNameExpanded += std::to_string( _index );

    return layoutNameExpanded;
}

void UiControlGroup::pushOffset() {
    mOffsetArrayInjectionStack = mOffsets;
    mZArrayInjectionStack = mUIZ;
}

void UiControlGroup::popOffset( int _index, int _numElements ) {
    if ( _numElements > 1 && _index != _numElements - 1 ) {
        mOffsets = mOffsetArrayInjectionStack;
        mUIZ = mZArrayInjectionStack;
    }
}

void UiControlGroup::advanceZ( const LayoutElement& _element, int /*ai*/ ) {
    // We always advance 2 to keep 1 space for elements and one underneath for background
    if ( _element.control == "UiBackground" ) {
        mUIZ+=2;
    }
}

void UiControlGroup::push_layout( const LayoutArrangement& _layoutArrangement ) {
    auto& l = _layoutArrangement.layout;

    Vector2f screen2dRatio = mIsScreenSpace ? getScreenAspectRatioVector : Vector2f::ONE;
    mRect = Rect2f{ Vector2f::ZERO, l.size * screen2dRatio };

    int maxArrayInjectionElements = getNumberOfArrayLayoutsFromInjection( l, _layoutArrangement );

    UiAlignElement layoutMargins = mMargins;
    UiAlignElement layoutArrangementMargins = _layoutArrangement.margins;
    layoutMargins.injectIfValid( layoutArrangementMargins );

    pushAnchor( mRect.size().x0(), layoutMargins );

    // This for loop is usually O(1), only time there are more iterations is when the same layout has hidden layouts
    // in an overlap position, useful normally for nice revealing animations
    for ( int ai = 0; ai < maxArrayInjectionElements; ai++ ) {

        std::string layoutNameExpanded = getLayoutName( l, ai, _layoutArrangement );

        pushOffset();

        for ( auto& element : l.elements ) {
            UiControlInitData cid;
            cid.control = element.control;
            cid.position = UiControl::stringToUiControlPosition(
                    element.position != "" ? element.position : _layoutArrangement.position );
            cid.sizeType = UiControl::stringToUiControlSizeType( element.sizeType );
            cid.size = element.size;
            cid.margins = l.margins;
            cid.padding = l.padding;

            cid.color = Color4f::FTORGBA( element.color );
            cid.backGroundColor = Color4f::FTORGBA( element.backGroundColor );
            cid.text = element.text;
            cid.title = element.title;
            cid.font = element.font;
            cid.flags = mFlags;
            for ( auto& f : element.flags ) {
                cid.flags |= UiControl::stringToUiControlFlag( f );
            }

            // check style override on sizeType
            overrideStyleIfNeeded( cid, uicm.getControlStyle( element.control ));
            overrideStyleIfNeeded( cid, uicm.getLayoutStyle( element.control + _layoutArrangement.layout.name ));
            overrideStyleIfNeeded( cid, uicm.getStyle(
                    element.control + _layoutArrangement.layout.name + _layoutArrangement.style ));

            cid.margins.injectIfValid( element.margins );
            cid.padding.injectIfValid( element.padding );

            // check injection
            std::string mkey =
                    cid.text + _layoutArrangement.style + ( maxArrayInjectionElements > 1 ? std::to_string( ai ) : "" );

            mMapping.injectValue( cid.text, mkey );
            mMapping.injectValue( cid.title, mkey );
            auto textInjArray =  mMapping.injectValueArrayFromFunction( cid.text );
            auto titleInjArray =  mMapping.injectValueArrayFromFunction( cid.title );

            // background overlap
            int backgroundZ = 0;
            ControlGroupAlignData alignData;
            for ( size_t i = 0; i < std::max(textInjArray.size(), titleInjArray.size()); i++ ) {
                cid.text = textInjArray[i < textInjArray.size() ? i : 0];
                cid.title = titleInjArray[i < titleInjArray.size() ? i : 0];
                cid.alpha = ( ai == maxArrayInjectionElements - 1 ) ? 1.0f : 0.0f;
                if ( cid.control == "UiBackground" ) {
                    backgroundZ = 1;
                    cid.position = UiControlPosition::Overlap;
                    cid.sizeType = UiControlSizeType::FullSize;
                    cid.size = fullSize();
                    alignData = { mAnchors.top().xy(), fullSize() };
                } else {
                    alignData = pushAlignment( cid.position, cid.sizeType, cid.size,
                                               cid.flags, cid.margins, cid.padding );
                }
                cid.params = { 1.0f, UIZ.level( mUIZ - backgroundZ ), 0.0f, 0.0f };
                auto c = mFactory.make(
                        { cid.control, alignData.origin, alignData.size, cid.color, cid.backGroundColor, cid.alpha,
                          element.name, cid.text, cid.title, cid.font, cid.flags, cid.params }, uicm.RR());
                c.first->setOnClickAction( mMapping.injectCallback( element.callback ) );
                group[layoutNameExpanded].push_back( std::move(c) );
            }

            advanceZ( element, ai );
        }

        popOffset( ai, maxArrayInjectionElements );
    }

    popAnchor( _layoutArrangement.position, layoutMargins );
}

Vector2f UiControlGroup::size() const {
    return mRect.size();
}

void UiControlGroup::isOnVR( const bool bVR ) {
    mbIsOnVR = bVR;
}

void UiControlGroup::addFlags( const int _flags ) {
    mFlags |= _flags;
}

void UiControlGroup::removeFlags( const int _flags ) {
    mFlags &= ~_flags;
}

void UiControlGroup::update( [[maybe_unused]] const float timeStep ) {

    for ( auto&& elem : *this ) {
        elem.second->update( mAnimMatrix );
    }

}

void UiControlGroup::AnimMatrix( const MatrixAnim& val ) {
    mAnimMatrix = val;
}

void UiControlGroup::UINormalAxis( const Vector3f& val ) {
    mUINormalAxis = val;
    for ( auto&& c : *this ) {
        c.second->UINormalAxis( val );
    }
}

void UiControlGroup::OrigRot( const Matrix4f& val ) {
    mOrigRot = val;
    for ( auto&& c : *this ) {
        c.second->OrigRot( val );
    }
}

//std::vector<std::unique_ptr<UiControl>>& UiControlGroup::getLayoutControls( const std::string& _name, int _index ) {
//	std::string iname = _name + std::to_string( _index );
//	ASSERT(group.find(iname) != group.end() );
//	return group[iname];
//}

int UiControlGroup::getNumbersOfLayoutControlsFor( const std::string& _name ) {
    int ret = 0;

    while ( group.find( _name + std::to_string( ret )) != group.end()) ++ret;

    return ret;
}

//std::vector<std::shared_ptr<VertexBuffer>> UiControlGroup::createVertexBuffers() {
//	std::vector<std::shared_ptr<VertexBuffer>> buffers;
//
//	size_t index = 0;
//	for ( auto& c : *this ) {
//        auto vb = c.second->createVertexBuffer( c.first );
//        vb->Id(index);
//        buffers.push_back(vb);
//        index++;
//	}
//	return buffers;
//}

void UiControlGroup::setVisible( bool visible ) {
    mVisible = visible;
    for ( auto && elem : *this ) {
        elem.first->setVisible( mVisible );
    }
}

void UiControlGroup::setEnabled( bool enabled ) {
    for ( auto && elem : *this ) {
        elem.first->setEnabled( enabled );
    }
}

void UiControlGroup::Selected( const bool val ) {
    for ( auto && elem : *this ) {
        elem.first->Selected( val );
    }
}

Vector2f UiControlGroup::fullSize() const {
    float lHeight = 1.0f;
    if ( mRect.height() == 0 ) {
        lHeight = mAnchors.top().y() - mOffsets.y();
    }
    return Vector2f{ mRect.size().x(), lHeight };
}

Vector2f UiControlGroup::fullSizeWithMargins( const UiAlignElement& _margins, const UiAlignElement& _padding ) const {
    float lHeight = ( mRect.height() - ( mRect.height() - mOffsets.y() ) ) - _margins.Height() - _padding.Height();
    return Vector2f{ mRect.size().x() - mOffsets.x() - _margins.Width() - _padding.Width(), lHeight };
}

Vector2f UiControlGroup::fullSizeQuadWithMargins( const UiAlignElement& _margins, const UiAlignElement& _padding ) const {
    if ( _margins.Size().dominantElement() == 1 ) {
        return Vector2f{ mRect.size().y() - _margins.Height() - _padding.Height() };
    }
    return Vector2f{ mRect.size().x() - _margins.Width() };
}

Vector2f UiControlGroup::fullWidthWithMargins( const float _heightPerc, const UiAlignElement& _margins,
                                               const UiAlignElement& _padding ) const {
    return Vector2f{ mRect.size().x() - _margins.Width() - _padding.Width(), _heightPerc };
}

Vector2f UiControlGroup::remainingWidthWithMargins( const float _heightPerc,
                                                    [[maybe_unused]] const UiAlignElement& _margins,
                                                    [[maybe_unused]] const UiAlignElement& _padding ) const {
    return Vector2f{ mRect.size().x() - mOffsets.x() + mStartPosition.x(), _heightPerc };
}

void UiControlGroup::changeTextOn( const std::string& _cname, const std::string& _newText ) {
    for ( auto& elem : *this ) {
        if ( elem.first->getName() == _cname ) {
            elem.first->setText( _newText );
            elem.second->make();
        }
    }
}

void UiControlGroup::changeTitleOn( const std::string& _cname, const std::string& _newText ) {
    for ( auto& elem : *this ) {
        if ( elem.first->getName() == _cname ) {
            elem.first->setTitle( _newText );
            elem.second->make();
        }
    }
}

UiControlFactory::UiControlFactory() {
    controlNameMap["UiBackground"] = std::make_unique<UiControlFactoryMaker<UiBackground, UiBackgroundRender>>();
    controlNameMap["UiLabel"     ] = std::make_unique<UiControlFactoryMaker<UiLabel, UiLabelRender>>();
    controlNameMap["UiImage"     ] = std::make_unique<UiControlFactoryMaker<UiImage, UiImageRender>>();
    controlNameMap["UiButton"    ] = std::make_unique<UiControlFactoryMaker<UiButton, UiButtonRender>>();
    controlNameMap["UiCheckBox"  ] = std::make_unique<UiControlFactoryMaker<UiCheckBox, UiCheckBoxRender>>();
    controlNameMap["UiSlider"    ] = std::make_unique<UiControlFactoryMaker<UiSlider, UiSliderRender>>();
    controlNameMap["UiInputBox"  ] = std::make_unique<UiControlFactoryMaker<UiInputBox, UiInputBoxRender>>();
    controlNameMap["UiSeparator" ] = std::make_unique<UiControlFactoryMaker<UiSeparator, UiSeparatorRender>>();
}

UiModelViewPair UiControlFactory::make( const UiControlBuilder& _data, Renderer& rr ) {
    if ( auto it = controlNameMap.find( _data.name ); it != controlNameMap.end()) {
        return it->second->get( _data, rr );
    }
    throw "UIControlFactory unknown type name";
}

bool UiViewBuilder::build( DependencyMaker& _md ) {
    UiControlManager& uicm = static_cast<UiControlManager&>(_md);

    if ( uicm.exists( Name() ) ) {
        return false;
    }
    readRemote<UiViewBuilder, HttpQuery::JSON, UIView>( Name(), *this, _md );
    return true;
}

//bool UiViewBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {
//
//    if  ( _status == DependencyStatus::LoadedSuccessfully ) {
//        UiControlManager& uicm = static_cast<UiControlManager&>(_md);
//        uicm.addUIView( UIView{ std::move(_data) } );
//        return true;
//    }
//
//    return false;
//}

UiViewBuilder::~UiViewBuilder() {

}

UiControlFactoryMakerI::~UiControlFactoryMakerI() {

}
