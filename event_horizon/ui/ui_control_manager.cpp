#include "ui_control_manager.hpp"

#include "ui_control_group.h"
#include "ui_background.h"
#include "../render_list.h"

std::map<std::string, Layout> UiControlManager::mLayoutsCache;

void UiControlManager::cmdLoadUi( const std::vector<std::string>& _params ) {
    UIVB{_params[0]}.build(*this);
}

void UiControlManager::update( float timeStep ) {
    // update all controls
    for ( auto& cg : mControlGroups ) {
        cg->update( timeStep );
    }
    // update text input output
    textInputUpdate();
}

void UiControlManager::textInputUpdate() {
    if ( !mStaticCenterScreenTextInputRenderer ) {
        mStaticCenterScreenTextInputRenderer = std::make_shared<UiTextInputRenderer>(rr);
    }
    if ( ti.GIsEnteringText ) {
        std::string ltext( TextInput::GInputString.begin(), TextInput::GInputString.end());
        mStaticCenterScreenTextInputRenderer->printTextInput( ti.TextInputBoxTitle(), ltext, rr.FM()() );
    } else {
        mStaticCenterScreenTextInputRenderer->clear();
    }
}

void UiControlManager::addControlGroup( std::shared_ptr<UiControlGroup> controlGroup ) {
    mControlGroups.push_back( controlGroup );
}

void UiControlManager::removeControlGroup( std::shared_ptr<UiControlGroup> controlGroup ) {
    if ( controlGroup ) {
        auto lid = controlGroup->Hash();
        mControlGroups.erase( remove_if( mControlGroups.begin(), mControlGroups.end(),
                                         [lid]( auto const& us ) -> bool { return us->Hash() == lid; } ),
                              mControlGroups.end());
        mLayoutsCache.clear();
    }
}

void UiControlManager::removeControlGroup( const std::string& _schemaName ) {
    mControlGroups.erase( remove_if( mControlGroups.begin(), mControlGroups.end(),
                                     [_schemaName]( auto const& us ) -> bool {
                                         return us->SchemaName() == _schemaName;
                                     } ), mControlGroups.end());
    mLayoutsCache.clear();
}

const Layout& UiControlManager::getLayout( const std::string& _name ) {
    return mLayouts[_name];
}

const StyleSheetElement& UiControlManager::getLayoutStyle( const std::string& _name ) const {
    static StyleSheetElement empty;
    auto ret = mLayoutStyles.find( _name );
    if ( ret != mLayoutStyles.end()) return ret->second;

    return empty;
}

const StyleSheetElement& UiControlManager::getControlStyle( const std::string& _name ) const {
    static StyleSheetElement empty;
    auto ret = mControlStyles.find( _name );
    if ( ret != mControlStyles.end()) return ret->second;

    return empty;
}

const StyleSheetElement& UiControlManager::getStyle( const std::string& _name ) const {
    static StyleSheetElement empty;
    auto ret = mStyles.find( _name );
    if ( ret != mStyles.end()) return ret->second;

    return empty;
}

void UiControlManager::addUIView( const UIView& _view ) {
    mViews[_view.name] = _view;
//    for ( const auto& l : _view.arrangements ) {
//
//    }

//    uicm.addStyles( view.style );
//    std::set<std::string> layoutNames;
//    for ( auto& arrangement : view.arrangements ) {
//        layoutNames.insert( arrangement.name );
//    }
//    for ( auto& ln : layoutNames ) {
//        uicm.addLayouts( ln );
//    }
}

void UiControlManager::addStyles( const std::string& uri ) {

    std::vector<StyleSheetElement> lStyles;
//    readFS<StyleSheetElement>( "StyleSheet", uri, "layout", lStyles );

    for ( auto style : lStyles ) {
        if ( style.control != "" && style.layout == "" && style.style == "" ) mControlStyles[style.control] = style;
        if ( style.control != "" && style.layout != "" && style.style == "" )
            mLayoutStyles[style.control + style.layout] = style;
        if ( style.control != "" && style.layout != "" && style.style != "" )
            mStyles[style.control + style.layout + style.style] = style;
    }
}

void UiControlManager::addLayouts( const std::string& uri ) {
    auto cl = mLayoutsCache.find( uri );

    if ( cl != mLayoutsCache.end()) {
        mLayouts[uri] = cl->second;
    } else {
        auto nl = Layout( uri );
        mLayouts[uri] = nl;
        mLayoutsCache[uri] = nl;
    }
}

TouchResult UiControlManager::onTouchDown( const Vector2f& pos, [[maybe_unused]] ModifiersKey mod ) {
    TouchResult touchDownResult = TOUCH_RESULT_NOT_HIT;

    for ( auto& cg : mControlGroups ) {
        for ( auto&& elem : *cg ) {
            auto&& it = elem.first;
            if ( it->isVisible() && it->isEnabled() && it->AcceptInputs() && it->isClickable()) {
                touchDownResult = it->checkTouchDown( pos );
                if ( touchDownResult == TOUCH_RESULT_HIT ) {
                    controlTouchedDown( it );
                    return touchDownResult;
                }
            }
        }
    }
    return touchDownResult;
}

void UiControlManager::onTouchMove( [[maybe_unused]] const Vector2f& pos, [[maybe_unused]] ModifiersKey mod ) {
    // Loop through all the controls

    for ( auto& cg : mControlGroups ) {
        for ( auto&& elem : *cg ) {
            auto&& it = elem.first;
            if ( it->isVisible() && it->isEnabled() && isControlTouchedDown( it )) {
                controlTouchedMoved( it );
            }
        }
    }
}

void UiControlManager::onTouchMoveM( float * /*x*/, float * /*y*/, int /*size*/, ModifiersKey /*mod*/ ) {
}

void UiControlManager::onTouchUp( [[maybe_unused]] const Vector2f& pos, [[maybe_unused]] ModifiersKey mod ) {
    for ( auto& cg : mControlGroups ) {
        for ( auto&& elem : *cg ) {
            auto&& it = elem.first;
            if ( it->isVisible() && it->isEnabled() && isControlTouchedDown( it )) {
                controlTouchedUp( it );
            }
        }
    }
}

void UiControlManager::controlTouchedDown( std::shared_ptr<UiControl>& _control ) {
    mTransientStates[_control->getId()].touchDownState = TOUCH_RESULT_HIT;
    if ( _control->isClickable()) {
        _control->touchDownAnimation();
    }
}

void UiControlManager::controlTouchedMoved( [[maybe_unused]] std::shared_ptr<UiControl>& _control ) {
}

void UiControlManager::controlTouchedUp( std::shared_ptr<UiControl>& _control ) {
//    if ( MI.wasTouchUpSingleEvent()) {
//        if ( _control->isClickable() /*&& !_control->AcceptInputs()*/ ) {
//            cq.enqueue( OnControlClickCommand( _control ));
//        }
//    }
    mTransientStates[_control->getId()].touchDownState = TOUCH_RESULT_NOT_HIT;
    _control->touchUpAnimation();
}

bool UiControlManager::isControlTouchedDown( const std::shared_ptr<UiControl>& _control ) const {
    if ( auto ret = mTransientStates.find( _control->getId());
    ret != mTransientStates.end()) {
        return ret->second.isTouchedDown();
    }
    return false;
}

const UIView& UiControlManager::getView( const std::string& _name ) const {
    return mViews.find(_name)->second;
}

int UiControlManager::ZLevelStack() const {
    return zStackLevel;
}

void UiControlManager::pushZLevelStack( int _zs) {
    zStackLevel = _zs;
}

void UiControlGroupBuilder::createDependencyList( DependencyMaker& _md ) {
    addDependency<UiViewBuilder>( Name(), _md );
    addDependencies( std::make_shared<UiControlGroupBuilder>(*this), _md );

//    UiControlManager& uicm = static_cast<UiControlManager&>(_md);
//    if ( removePolicy == RemovePolicy::doRemoveOld ) uicm.removeControlGroup( name );
}

void UiControlGroupBuilder::assemble( DependencyMaker& _md ) {

    UiControlManager& uicm = static_cast<UiControlManager&>(_md);

    auto ret = std::make_shared<UiControlGroup>(uicm, Name(), flags, kvmapping);
    uicm.addControlGroup( ret );

//    auto t1 = std::make_shared<AnimTimeline>();
//    for ( auto& elem : *ret.get() ) {
//        t1->addToStream( 5.f, 0.0f, 0.01f, 1.0f, [&]( float a ) {
////            elem.second->getAnimMatrix().pos->value = Vector3f{ 1.0f, a, 1.0f };
//            elem.second->getAnimMatrix().scale->value = Vector3f{ 1.0f, a, 1.0f };
//        } );
//    }
//    AnimUpdateTimeline::startTimeline( t1 );

//    static Vector3f sposgc = Vector3f::ONE*0.4f;
//    for ( auto& elem : *ret ) {
//        elem.first->PositionAnim()->value += sposgc;
//    }
//    sposgc+= Vector3f::X_AXIS * 0.001f;

}

bool UiControlGroupBuilder::validate() const {
    return !Name().empty();
}

