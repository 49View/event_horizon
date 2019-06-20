//
// Created by Dado on 2019-05-28.
//

#pragma once

#include <cstdint>
#include <string>

#include <core/math/rect2f.h>
#include <core/math/matrix_anim.h>
#include <core/recursive_transformation.hpp>
#include <graphics/ghtypes.hpp>
#include <poly/poly.hpp>

class UIView;
class SceneGraph;
class RenderOrchestrator;

enum class UITapAreaStatus {
    Enabled,
    Disabled,
    Selected,
    Hidden,
    Hoover,
    Fixed,
};

using UITS = UITapAreaStatus;

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

struct UIKey {
    template<typename ...Args>
    explicit UIKey( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    ResourceRef operator()() const noexcept {
        return data;
    }
    ResourceRef data;
};

struct UITapAreaType {
    template<typename ...Args>
    explicit UITapAreaType( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    uint64_t operator()() const noexcept {
        return data;
    }
    uint64_t data = 0;
};

namespace UIT {
    static const UITapAreaType background   = UITapAreaType{1 << 0};
    static const UITapAreaType pushButton   = UITapAreaType{1 << 1};
    static const UITapAreaType stickyButton = UITapAreaType{1 << 2};
    static const UITapAreaType label        = UITapAreaType{1 << 3};
}

struct UIFontRef {
    template<typename ...Args>
    explicit UIFontRef( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    std::string operator()() const noexcept {
        return data;
    }
    std::string data;
};

struct UIFontAngle {
    template<typename ...Args>
    explicit UIFontAngle( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    float operator()() const noexcept {
        return data;
    }
    float data;
};

struct UIFontText {
    template<typename ...Args>
    explicit UIFontText( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    std::string operator()() const noexcept {
        return data;
    }
    std::string data;
};

struct UIForegroundIcon {
    template<typename ...Args>
    explicit UIForegroundIcon( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    std::string operator()() const noexcept {
        return data;
    }
    std::string data;
};

class UIElement : public Boxable<> {
public:
    template <typename ...Args>
    explicit UIElement( Args&& ... args ) {
        (parseParam( std::forward<Args>( args )), ...); // Fold expression (c++17)
        if ( type() == UIT::background() || type() == UIT::label() ) {
            status = UITS::Fixed;
        }
        foregroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "ForeGroundButtonColor" );
        backgroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "BackGroundButtonColor" );
    }
private:
    template<typename M>
    void parseParam( const M& _param ) {
        if constexpr ( std::is_same_v<M, UITapAreaType > ) {
            type = _param;
        }
        if constexpr ( std::is_same_v<M, JMATH::Rect2f > ) {
            bbox = _param;
        }
        if constexpr ( std::is_same_v<M, UITapAreaStatus > ) {
            status = _param;
        }
        if constexpr ( std::is_same_v<M, UIFontRef > ) {
            fontRef = _param();
        }
        if constexpr ( std::is_same_v<M, UIFontText > ) {
            text = _param();
        }
        if constexpr ( std::is_same_v<M, UIFontAngle > ) {
            fontAngle = _param();
        }
        if constexpr ( std::is_same_v<M, UIForegroundIcon > ) {
            foreground = _param();
        }
        if constexpr ( std::is_pointer_v<M> ) {
            owner = _param;
        }
    }

public:
    uint64_t Type() const {
        return type();
    }

    const Rect2f& Area() const {
        return bbox;
    }

    UITapAreaStatus Status() const {
        return status;
    }

    const ResourceRef& FontRef() const {
        return fontRef;
    }

    const std::string& Text() const {
        return text;
    }

    const std::string& Foreground() const {
        return foreground;
    }

    void Owner( UIView* _elem ) {
        owner = _elem;
    }

    void Font( const Font* _fontPtr ) {
        font = _fontPtr;
    }

    bool contains( const V2f& _point ) const {
        return bbox.contains( _point );
    }
    void touchedDown();
    void touchedUp( bool hasBeenTapped, bool isTouchUpGroup );
    void transform( float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void loadResource( CResourceRef _idb );
    void loaded() { ready = true; }
    void hoover( bool isHoovering );
    void setStatus( UITapAreaStatus _status );

private:
    UITapAreaType   type;
    Rect2f          bbox   = Rect2f::INVALID;
    UITapAreaStatus status = UITapAreaStatus::Enabled;
    const ::Font*   font = nullptr;
    std::string     fontRef;
    std::string     text;
    float           fontAngle = 0.0f;

    std::string     foreground;

    UIView*         owner = nullptr;
    bool            ready = false;
    std::string     background;
    std::string     touchDownAnimNamePos;
    std::string     touchDownAnimNameScale;
    VPListSP        foregroundVP;
    VPListSP        backgroundVP;
    VPListSP        shadowVP;
    MatrixAnim      foregroundAnim;
    MatrixAnim      backgroundAnim;
    V4fa            foregroundColor;
    V4fa            backgroundColor;
};

using UIElementRT = RecursiveTransformation<UIElement, JMATH::AABB>;
using UIElementSP = std::shared_ptr<UIElementRT>;

class UIView {
public:
    UIView( SceneGraph& sg, RenderOrchestrator& rsg, const ColorScheme& cs ) : sg( sg ), rsg( rsg ), colorScheme(cs) {}

    void add( CResourceRef _key, UIElementSP _elem );

    ResourceRef isTapInArea( const V2f& tap ) const;
    bool isTouchDownInside( const V2f& _p );
    void handleTouchDownUIEvent( const V2f& _p );
    UIElementSP TapArea( CResourceRef _key );
    void touchDownKeyCached( CResourceRef _key ) const;
    void touchedUp( CResourceRef _key );
    CResourceRef touchDownKeyCached() const;
    void loadResources();
    void loaded( CResourceRef _key );
    void hoover( CResourceRef _key );
    void transform( CResourceRef _key, float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void setButtonStatus( CResourceRef _key, UITapAreaStatus _status );
    UITapAreaStatus getButtonStatus( CResourceRef _key ) const;
    bool isButtonEnabled( CResourceRef _key ) const;

    Renderer& RR();
    SceneGraph& SG();

    C4f getEnabledColor() const;
    C4f getSelectedColor() const;
    C4f getDisabledColor() const;
    C4f getHooverColor() const;
    C4f getPressedDownColor() const;

    C4f colorFromStatus( UITapAreaStatus _status );

private:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    ColorScheme colorScheme;
    std::unordered_map<ResourceRef, UIElementSP> tapAreas;
    mutable ResourceRef touchDownStartingKey;
};



