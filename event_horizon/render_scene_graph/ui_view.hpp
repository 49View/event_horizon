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

class ColorScheme {
public:
    static constexpr size_t ShadesNum = 5;
    ColorScheme( const std::string& colorDescriptor );

    C4f Primary( int index = 0 ) const {
        return primaryColors[index];
    }
    C4f Secondary1( int index = 0 ) const {
        return secondary1Colors[index];
    }
    C4f Secondary2( int index = 0 ) const {
        return secondary2Colors[index];
    }
    C4f Complement( int index = 0 ) const {
        return complementColors[index];
    }
private:
    std::array<C4f, ShadesNum> primaryColors;
    std::array<C4f, ShadesNum> secondary1Colors;
    std::array<C4f, ShadesNum> secondary2Colors;
    std::array<C4f, ShadesNum> complementColors;
};

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
    UIView( SceneGraph& sg, RenderOrchestrator& rsg, const ColorScheme& cs ) : sg( sg ), rsg( rsg ), colorScheme(cs) {}

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

    C4f getEnabledColor() const {
        return colorScheme.Primary(0);
    }

    C4f getSelectedColor() const {
        return colorScheme.Primary(3);
    }

    C4f getDisabledColor() const {
        return colorScheme.Primary(4);
    }

    C4f getHooverColor() const {
        return colorScheme.Primary(2);
    }

    C4f getPressedDownColor() const {
        return colorScheme.Primary(3);
    }

//    const std::string& getShadowBackground() const {
//        return shadowBackground;
//    }

    C4f colorFromStatus( UITapAreaStatus _status );

private:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    ColorScheme colorScheme;
    std::unordered_map<uint64_t, std::shared_ptr<UITapArea>> tapAreas;
    mutable uint64_t touchDownStartingKey = 0;
};



