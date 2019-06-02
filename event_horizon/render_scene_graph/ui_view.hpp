//
// Created by Dado on 2019-05-28.
//

#pragma once

#include <cstdint>
#include <string>

#include <core/math/rect2f.h>
#include <core/math/matrix_anim.h>
#include <graphics/ghtypes.hpp>

class UIView;
class SceneGraph;
class RenderOrchestrator;

enum class UITapAreaStatus {
    Enabled,
    Disabled,
    Selected,
    Hidden,
    Hoover
};

namespace UITapAreaType {
    constexpr uint64_t pushButton = 1;
    constexpr uint64_t stickyButton = 1 << 2;
}

struct UITapArea {
    UITapArea(  UIView* _owner, const Rect2f& _area, uint64_t _type, UITapAreaStatus _status,
                std::string _foreground, std::string _background );

    bool contains( const V2f& _point ) const {
        return area.contains( _point );
    }
    void touchedDown();
    void touchedUp( bool hasBeenTapped, bool isTouchUpGroup );
    void transform( float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void loadResource( uint64_t _idb );
    void loaded() { ready = true; }
    void hoover( bool isHoovering );
    void setStatus( UITapAreaStatus _status );

    UIView* owner = nullptr;
    Rect2f  area = Rect2f::MIDENTITY();
    bool    ready = false;
    uint64_t  type;
    UITapAreaStatus status = UITapAreaStatus::Enabled;
    std::string foreground;
    std::string background;
    std::string touchDownAnimNamePos;
    std::string touchDownAnimNameScale;
    VPListSP foregroundVP;
    VPListSP backgroundVP;
    VPListSP shadowVP;
    MatrixAnim foregroundAnim;
    MatrixAnim backgroundAnim;
    V4fa foregroundColor;
    V4fa backgroundColor;
};

class UIView {
public:
    UIView( SceneGraph& sg, RenderOrchestrator& rsg ) : sg( sg ), rsg( rsg ) {}

    void addButton( uint64_t _key, const Rect2f& _area, uint64_t _type, UITapAreaStatus _status,
                    std::string _foreground, std::string _background, const V3f& _initialPos = V3f::ZERO );
    uint64_t isTapInArea( const V2f& tap ) const;
    bool isTouchDownInside( const V2f& _p );
    void handleTouchDownUIEvent( const V2f& _p );
    std::shared_ptr<UITapArea> TapArea( uint64_t _key ) const;
    void touchDownKeyCached( uint64_t _key ) const;
    void touchedUp( uint64_t _key );
    uint64_t touchDownKeyCached() const;
    void loadResources();
    void loaded( uint64_t _key );
    void hoover( uint64_t _key );
    void transform( uint64_t _key, float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void setButtonStatus( uint64_t _key, UITapAreaStatus _status );
    UITapAreaStatus getButtonStatus( uint64_t _key ) const;
    bool isButtonEnabled( uint64_t _key ) const;

    Renderer& RR();
    SceneGraph& SG();

    const C4f& getEnabledColor() const {
        return enabledColor;
    }

    const C4f& getSelectedColor() const {
        return selectedColor;
    }

    const C4f& getDisabledColor() const {
        return disabledColor;
    }

    const C4f& getHooverColor() const {
        return hooverColor;
    }

    const C4f& getPressedDownColor() const {
        return pressedDownColor;
    }

//    const std::string& getShadowBackground() const {
//        return shadowBackground;
//    }

    C4f colorFromStatus( UITapAreaStatus _status );

private:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    std::unordered_map<uint64_t, std::shared_ptr<UITapArea>> tapAreas;
    mutable uint64_t touchDownStartingKey = 0;
//    bool backGroundShadowLoaded = false;
//    std::string shadowBackground = "button,square,shadow";

    C4f enabledColor = V4f::XTORGBA("f2a571");
    C4f selectedColor = V4f::XTORGBA("ffe68e");
    C4f disabledColor = V4f::XTORGBA("b5b3ad");
    C4f hooverColor = V4f::XTORGBA("cea71a");
    C4f pressedDownColor = V4f::XTORGBA("ed7525");
};



