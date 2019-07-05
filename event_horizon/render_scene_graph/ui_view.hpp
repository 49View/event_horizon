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
#include <core/resources/entity_factory.hpp>

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

template<class T>
struct is_c_str
        : std::integral_constant<
                bool,
                std::is_same_v<char const *, typename std::decay_t<T>> ||
                std::is_same_v<char *, typename std::decay_t<T>>
        > {};

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
    static const UITapAreaType separator_h  = UITapAreaType{1 << 4};
    static const UITapAreaType separator_v  = UITapAreaType{1 << 5};
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
    UIFontText( const ResourceRef& fontRef, float height, const std::string& text ) : fontRef( fontRef ),
                                                                                      height( height ), text( text ) {}

    ResourceRef fontRef;
    float height = 0.1f;
    std::string text;
};

struct UIForegroundIcon {
    template<typename ...Args>
    explicit UIForegroundIcon( Args&& ... args ) : data(std::forward<Args>( args )...) {}
    std::string operator()() const noexcept {
        return data;
    }
    std::string data;
};

class UIElement : public Boxable<JMATH::AABB, BBoxProjection2d> {
public:
    template <typename ...Args>
    explicit UIElement( Args&& ... args ) {
        bbox3d->identity();
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
        if constexpr ( std::is_same_v<M, std::string > || is_c_str<M>::value ) {
            key = _param;
        }

//        if constexpr ( std::is_same_v<M, V3f > || std::is_same_v<M, V2f > ) {
//            pos = _param;
//        }
//        if constexpr ( std::is_same_v<M, Quaternion > ) {
//            rot = _param;
//        }
//        if constexpr ( std::is_same_v<M, MScale > ) {
//            scale = _param();
//        }
        if constexpr ( std::is_same_v<M, MScale2d > ||
                std::is_same_v<M, MScale2dXS > ||
                std::is_same_v<M, MScale2dYS > ||
                std::is_same_v<M, MScale2dXYS >) {
            bbox3d->scaleX( _param().x() );
            bbox3d->scaleY( _param().y() );
            bbox3d->scaleZ( 0.0f );
        }
        if constexpr ( std::is_same_v<M, UITapAreaType > ) {
            type = _param;
        }
//        if constexpr ( std::is_same_v<M, JMATH::Rect2f > ) {
//            bbox = _param;
//        }
        if constexpr ( std::is_same_v<M, UITapAreaStatus > ) {
            status = _param;
        }
        if constexpr ( std::is_same_v<M, UIFontRef > ) {
            fontRef = _param();
        }
        if constexpr ( std::is_same_v<M, UIFontText > ) {
            fontRef = _param.fontRef;
            text = _param.text;
            fontHeight = _param.height;
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

    std::string Key() const {
        return key;
    }

    Rect2f Area() const {
        return bbox3d->topDown();
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

    [[nodiscard]] bool contains( const V2f& _point ) const;
    [[nodiscard]] bool containsActive( const V2f& _point ) const;
    [[nodiscard]] bool hasActiveStatus() const;
    void touchedDown();
    void touchedUp( bool hasBeenTapped, bool isTouchUpGroup );
    void transform( float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void loadResource( std::shared_ptr<Matrix4f> _localHierMat );
    void hoover( bool isHoovering );
    void setStatus( UITapAreaStatus _status );

private:
    std::string     key;
    UITapAreaType   type;
//    V3f             pos = V3f::ZERO;
//    Quaternion      rot;
//    V3f             scale = V3f::ONE;
//    Rect2f          bbox   = Rect2f::IDENTITY;
    UITapAreaStatus status = UITapAreaStatus::Enabled;
    const ::Font*   font = nullptr;
    std::string     fontRef;
    std::string     text;
    float           fontAngle = 0.0f;
    float           fontHeight = 0.2f;

    std::string     foreground;

    UIView*         owner = nullptr;
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
    C4f             defaultBackgroundColor = C4f::WHITE;
};

using UIElementRT       = RecursiveTransformation<UIElement, JMATH::AABB>;
using UIElementSP       = std::shared_ptr<UIElementRT>;
using UIElementSPConst  = std::shared_ptr<const UIElementRT>;
using UIElementSPCC     = const UIElementSPConst;

enum class UICheckActiveOnly {
    False,
    True
};

class UIView {
public:
    UIView( SceneGraph& sg, RenderOrchestrator& rsg, const ColorScheme& cs ) : sg( sg ), rsg( rsg ), colorScheme(cs) {}

    void add( UIElementSP _elem );

    void handleTouchDownEvent( const V2f& _p );
    void handleTouchUpEvent( const V2f& _p );
    void loadResources();
    void hoover(  const V2f& _point );
    void transform( UIElementSP _key, float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void setButtonStatus( CResourceRef _key, UITapAreaStatus _status );
    UITapAreaStatus getButtonStatus( CResourceRef _key ) const;
    bool isButtonEnabled( CResourceRef _key ) const;
    bool isHandlingUI() const;
    [[nodiscard]] bool pointInUIArea( const V2f& tap, UICheckActiveOnly _checkFlag ) const;

    Renderer& RR();
    SceneGraph& SG();

    C4f getEnabledColor() const;
    C4f getSelectedColor() const;
    C4f getDisabledColor() const;
    C4f getHooverColor() const;
    C4f getPressedDownColor() const;

    C4f colorFromStatus( UITapAreaStatus _status );

    void visit( std::function<void(const UIElementSPConst)> f ) const ;
    void foreach( std::function<void(UIElementSP)> f );

private:
    void touchDownKeyCached( UIElementSP _key ) const;
    UIElementSP touchDownKeyCached() const;
    void addRecursive( UIElementSP _elem );

private:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    ColorScheme colorScheme;
    std::vector<UIElementSP> elements;
    std::vector<UIElementSP> activeTaps;
    mutable UIElementSP touchDownStartingKey;
};

enum class CSSDisplayMode {
    Block,
    Inline
};

using ControlDefKey = std::string;
using ControlDefIconRef = std::string;
using ControlSingleTapCallback = std::function<void(ControlDefKey)>;

struct ControlDef {
    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon, const std::vector<UIFontText>& textLines )
            : key( key ), icon( icon ), textLines( textLines ) {}

    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon ) : key( key ), icon( icon ) {}

    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon,
                const ControlSingleTapCallback& singleTapCallback ) : key( key ), icon( icon ),
                                                                      singleTapCallback( singleTapCallback ) {}

    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon, const std::vector<UIFontText>& textLines,
                const ControlSingleTapCallback& singleTapCallback ) : key( key ), icon( icon ), textLines( textLines ),
                                                                      singleTapCallback( singleTapCallback ) {}

    ControlDefKey key;
    ControlDefIconRef icon;
    std::vector<UIFontText> textLines;
    ControlSingleTapCallback singleTapCallback;
};

class UIContainer2d {
public:
    template <typename S>
    UIContainer2d( UIView& _owner, const MPos2d& _pos, const S& _size ) : owner(_owner), pos( _pos ), size( _size() ) {
        node = EF::create<UIElementRT>( PFC{}, UUIDGen::make(), pos, _size, UIT::background );
        innerPaddedX = size.x()-(padding.x()*2.0f);
        caret = padding;
        wholeLineSize = MScale2d{innerPaddedX, -padding.y()};
    }

    void addEmptyCaret();
    void addEmptyCaretNewLine();
    void addTitle( const UIFontText& _text );
    void addListEntry( const ControlDef& _cd );
    void addListEntryGrid( const ControlDef& _cd, bool _newLine = false );
    void addButtonGroupLine( UITapAreaType _uit, const std::vector<ControlDef>& _cds );
    void popCaretX();
    void finalise();

private:
    void advanceCaret( CSSDisplayMode _displayMode, const MScale2d& _elemSize );
    void addSeparator( float percScaleY = 1.0f );
    void addLabel( const UIFontText& _text, const MScale2d& bsize, CSSDisplayMode displayMode,
                   const V2f& _pos = V2f::ZERO );
    UIElementSP addButton( const ControlDef& _cd, const MScale2d& bisze, CSSDisplayMode displayMode,
                    UITapAreaType _bt = UIT::pushButton, const V2f& _pos = V2f::ZERO );

private:
    UIView& owner;
    MPos2d pos;
    V2f padding{0.02f, -0.01f};
    V3f size;
    UIElementSP node;

    std::unordered_map<std::string, UIElementSP> icontrols;
    float innerPaddedX = 0.0f;
    V2f caret{V2f::ZERO};
    std::vector<V2f> caretQueue;
    MScale2d wholeLineSize{0.0f, 0.0f};
};

