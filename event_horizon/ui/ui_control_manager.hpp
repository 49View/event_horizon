//
//  ui_control_manager_hpp
//
//

#pragma once

#include "core/kvfmapping.h"
#include "core/callback_dependency.h"
#include "core/command.hpp"
#include "core/htypes_shared.hpp"
#include "../di_modules.h"
#include "../text_input.hpp"
#include "../mouse_input.hpp"
#include "ui_bsdata.hpp"

class Vector2f;
class UiControlGroup;
class UiControl;
class UiTextInputRenderer;

struct UiControlState {
	bool isTouchedDown() const { return touchDownState == TOUCH_RESULT_HIT; }

	TouchResult touchDownState;
};

enum RemovePolicy {
	dontRemoveOld,
	doRemoveOld
};

class UiControlManager;


class UiControlManager : public DependencyMaker {
public:
    virtual ~UiControlManager() = default;
	UiControlManager( Renderer& rr, TextInput& ti ) : rr( rr ), ti( ti) {}

	void update( float timeStep );

    void cmdLoadUi( const std::vector<std::string>& _params );

    TouchResult onTouchDown( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE );
	void onTouchMove( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE );
	void onTouchMoveM( float* x, float *y, int size, ModifiersKey mod = GMK_MOD_NONE );
	void onTouchUp( const Vector2f& pos, ModifiersKey mod = GMK_MOD_NONE );

	void removeControlGroup( std::shared_ptr<UiControlGroup> controlGroup );
	void removeControlGroup( const std::string& _schemaName );
	void addControlGroup( std::shared_ptr<UiControlGroup> controlGroup );
	int ZLevelStack() const;
	void pushZLevelStack( int _zs);

	void addUIView( const UIView& _view );
	void addLayouts( const std::string& uri );
	void addStyles( const std::string& uri );

	const UIView& getView( const std::string& _name ) const;
	const Layout& getLayout( const std::string& _name );
	const StyleSheetElement& getLayoutStyle( const std::string& _name ) const;
	const StyleSheetElement& getControlStyle( const std::string& _name ) const;
	const StyleSheetElement& getStyle( const std::string& _name ) const;

	Renderer& RR() { return rr; }
private:

	bool isControlTouchedDown( const std::shared_ptr<UiControl>& _control ) const;
	void controlTouchedDown( std::shared_ptr<UiControl>& _control );
	void controlTouchedUp( std::shared_ptr<UiControl>& _control );
	void controlTouchedMoved( std::shared_ptr<UiControl>& _control );

    void textInputUpdate();
private:
	std::vector<std::shared_ptr<UiControlGroup>> mControlGroups;
	std::map< std::string, UIView> mViews;
	std::map< std::string, Layout> mLayouts;
	static std::map< std::string, Layout> mLayoutsCache;
	std::map< std::string, StyleSheetElement > mLayoutStyles;
	std::map< std::string, StyleSheetElement > mControlStyles;
	std::map< std::string, StyleSheetElement > mStyles;
	int zStackLevel = 2; // Starts from 2, 1 is for UI background and zero for rest of graphics

	std::shared_ptr<UiTextInputRenderer> mStaticCenterScreenTextInputRenderer;

    std::map< uint64_t, UiControlState > mTransientStates;
    Renderer& rr;
	TextInput& ti;
};

class UiControlGroupBuilder : public DependantBuilder {
	using DependantBuilder::DependantBuilder;
public:
	UiControlGroupBuilder() {}
	UiControlGroupBuilder( const UiControlGroupBuilder& _b ) : DependantBuilder(_b.Name()) {
		flags = _b.flags;
		kvmapping = _b.kvmapping;
		rootPosition = _b.rootPosition;
		removePolicy = _b.removePolicy;
	}

	virtual ~UiControlGroupBuilder() {}
public:
	void assemble() override;

	UiControlGroupBuilder& f( const UiControlFlag _flags ) {
		orBitWiseFlag( flags, _flags );
		return *this;
	}

protected:
	void elemCreate() override {}
	bool validate() const override;
	void createDependencyList() override;
private:
	UiControlFlag flags = UiControlFlag::UseScreenSpace;
	KVFMapping    kvmapping;
	Vector3f      rootPosition;
	RemovePolicy  removePolicy;
};

using UIVB = UiControlGroupBuilder;