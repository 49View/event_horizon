//
// Created by Dado on 2019-05-28.
//

#pragma once

#include <cstdint>
#include <string>

#include <core/math/rect2f.h>
#include <core/math/matrix_anim.h>
#include <core/recursive_transformation.hpp>
#include <core/resources/entity_factory.hpp>
#include <core/resources/ui_container.hpp>
#include <poly/poly.hpp>
#include <graphics/ghtypes.hpp>
#include <render_scene_graph/render_orchestrator.h>

class UIView;
class SceneGraph;
class RenderOrchestrator;

struct UICallbackHandle {
    UICallbackHandle() {}
    UICallbackHandle( const std::string& _index ) {
        index = std::stoi( _index );
    }
    UICallbackHandle( int _index ) : index( _index ) {}

    int index = -1;
};

using ControlDefKey = std::string;
using ControlTapKey = uint64_t;
using ControlDefIconRef = std::string;
using UICallbackFunc = std::function<void( const UICallbackHandle& )>;
using ControlSingleTapCallback = UICallbackFunc;//std::function<void( ControlTapKey )>;
using UICallbackMap  = std::unordered_map<std::string, UICallbackFunc>;
const static auto sUIEmptyCallback = []( const UICallbackHandle& ) { LOGRS("[WARNING] void UI callback called")};
const static auto ControlTapCallbackEmpty = sUIEmptyCallback;//[]( ControlTapKey ) {};

enum class UIElementStatus {
    Enabled,
    Disabled,
    Selected,
    Hidden,
    Hoover,
    Fixed,
};

enum class CSSDisplayMode {
    Block,
    Inline
};

enum UIAlignFlags {
    UIAF_None                       = 0,
    UIAF_DoNotAddSeparator          = 1 << 0,
};

template<class T>
struct is_c_str
        : std::integral_constant<
                bool,
                std::is_same_v<char const *, typename std::decay_t<T>> ||
                std::is_same_v<char *, typename std::decay_t<T>>
        > {
};

struct UIKey {
    template<typename ...Args>
    explicit UIKey( Args&& ... args ) : data( std::forward<Args>( args )... ) {}

    ResourceRef operator()() const noexcept {
        return data;
    }

    ResourceRef data;
};

struct UITapAreaType {
    template<typename ...Args>
    explicit UITapAreaType( Args&& ... args ) : data( std::forward<Args>( args )... ) {}

    uint64_t operator()() const noexcept {
        return data;
    }

    uint64_t data = 0;
};

namespace UIT {
    static const UITapAreaType background = UITapAreaType{ 1 << 0 };
    static const UITapAreaType pushButton = UITapAreaType{ 1 << 1 };
    static const UITapAreaType stickyButton = UITapAreaType{ 1 << 2 };
    static const UITapAreaType label = UITapAreaType{ 1 << 3 };
    static const UITapAreaType separator_h = UITapAreaType{ 1 << 4 };
    static const UITapAreaType separator_v = UITapAreaType{ 1 << 5 };
    static const UITapAreaType toggleButton = UITapAreaType{ 1 << 6 };
}

struct UIFontRef {
    template<typename ...Args>
    explicit UIFontRef( Args&& ... args ) : data( std::forward<Args>( args )... ) {}

    std::string operator()() const noexcept {
        return data;
    }

    std::string data;
};

struct UIFontAngle {
    template<typename ...Args>
    explicit UIFontAngle( Args&& ... args ) : data( std::forward<Args>( args )... ) {}

    float operator()() const noexcept {
        return data;
    }

    float data;
};

struct UIFontText {
    UIFontText( ResourceRef fontRef, float height, std::string text ) : fontRef( std::move( fontRef )),
                                                                        height( height ), text( std::move( text )) {}

    UIFontText( ResourceRef fontRef, float height, const C4f& color, std::string text ) : fontRef( std::move(
            fontRef )), height( height ), color( color ), text( std::move( text )) {}

    ResourceRef fontRef;
    float height = 0.1f;
    C4f color = C4f::WHITE;
    std::string text;
};

struct UIForegroundIcon {
    template<typename ...Args>
    explicit UIForegroundIcon( Args&& ... args ) : data( std::forward<Args>( args )... ) {}

    std::string operator()() const noexcept {
        return data;
    }

    std::string data;
};

struct ControlDef {
    ControlDef( ControlDefKey key, ControlDefIconRef icon, std::vector<UIFontText> textLines )
            : key( std::move( key )), icon( std::move( icon )), textLines( std::move( textLines )) {}

    ControlDef( ControlDefKey key, ControlDefIconRef icon ) : key( std::move( key )), icon( std::move( icon )) {}

    ControlDef( ControlDefKey key, ControlDefIconRef icon,
                ControlSingleTapCallback singleTapCallback ) : key( std::move( key )), icon( std::move( icon )),
                                                               singleTapCallback( std::move( singleTapCallback )) {}

    ControlDef( ControlDefKey key, ControlDefIconRef icon,
                ControlSingleTapCallback cbOn, ControlSingleTapCallback cbOff ) : key( std::move( key )),
                                                                                  icon( std::move( icon )),
                                                                                  singleTapCallback( std::move( cbOn )),
                                                                                  singleTapOffToggleCallback(
                                                                                          std::move( cbOff )) {}

    ControlDef( ControlDefKey key, ControlDefIconRef icon, const UIFontText& textLine,
                ControlSingleTapCallback singleTapCallback, const UICallbackHandle& _cbParam = {} ) : key( std::move( key )), icon( std::move( icon )),
                                                               singleTapCallback( std::move( singleTapCallback )), cbParam(_cbParam) {
        textLines.push_back( textLine );
    }

    ControlDef( ControlDefKey key, ControlDefIconRef icon, std::vector<UIFontText> textLines,
                ControlSingleTapCallback singleTapCallback, const UICallbackHandle& _cbParam = {} ) : key( std::move( key )), icon( std::move( icon )),
                                                               textLines( std::move( textLines )),
                                                               singleTapCallback( std::move( singleTapCallback )),
                                                               cbParam(_cbParam){}


    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon, const std::vector<UIFontText>& textLines,
                const ControlSingleTapCallback& singleTapCallback, const Color4f& tintColor ) : key( key ),
                                                                                                icon( icon ),
                                                                                                textLines( textLines ),
                                                                                                singleTapCallback(
                                                                                                        singleTapCallback ),
                                                                                                tintColor(
                                                                                                        tintColor ) {}

    ControlDef( const ControlDefKey& key, const ControlDefIconRef& icon, const std::vector<UIFontText>& textLines,
                const Color4f& tintColor ) : key( key ), icon( icon ), textLines( textLines ), tintColor( tintColor ) {}
//    ControlDef( ControlDefKey  key, ControlDefIconRef  icon, std::vector<UIFontText>  textLines,
//                const Color4f& _color ) : key(std::move( key )), icon(std::move( icon )), textLines(std::move( textLines )),
//                                                   tintColor( _color ) {}

//    ControlDef( ControlDefKey  key, ControlDefIconRef  icon, std::vector<UIFontText>  textLines,
//                const Color4f& foreGroundColor, const Color4f& backGroundColor ) : key(std::move( key )), icon(std::move( icon )),
//                                                                                   textLines(std::move( textLines )),
//                                                                                   foreGroundColor( foreGroundColor ),
//                                                                                   backGroundColor( backGroundColor ) {}

    ControlDefKey key;
    ControlDefIconRef icon;
    std::vector<UIFontText> textLines;
    ControlSingleTapCallback singleTapCallback = ControlTapCallbackEmpty;
    ControlSingleTapCallback singleTapOffToggleCallback = ControlTapCallbackEmpty;
    Color4f tintColor = C4f::WHITE;
    UICallbackHandle cbParam;
};

using UITS = UIElementStatus;

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

class UIElement;

using UIElementRT       = RecursiveTransformation<UIElement, JMATH::AABB>;
using UIElementSP       = std::shared_ptr<UIElementRT>;
using UIElementSPConst  = std::shared_ptr<const UIElementRT>;
using UIElementSPCC     = const UIElementSPConst;

class UIElement : public Boxable<JMATH::AABB, BBoxProjection2d>, public UUIDIntegerInc {
public:
    template<typename ...Args>
    explicit UIElement( RenderOrchestrator& _rsg, Args&& ... args ) : UUIDIntegerInc( CommandBufferLimits::UI2dStart ),
                                                                      rsg( _rsg ) {
        bbox3d->identity();
        (parseParam( std::forward<Args>( args )), ...); // Fold expression (c++17)
        if ( type() == UIT::background() || type() == UIT::label()) {
            status = UITS::Fixed;
        }

        backgroundColor = std::make_shared<AnimType<V4f>>( Vector4f::ZERO, "BackGroundButtonColor" );
    }

private:
    template<typename M>
    void parseParam( const M& _param ) {
        if constexpr ( std::is_same_v<M, ControlDef> ) {
            key = _param.key;
            singleTapCallback = _param.singleTapCallback;
            singleTapOffToggleCallback = _param.singleTapOffToggleCallback;
        }

        if constexpr ( std::is_same_v<M, std::string> || is_c_str<M>::value ) {
            key = _param;
        }

        if constexpr ( std::is_same_v<M, UICallbackHandle> ) {
            cbParam = _param;
        }

        if constexpr ( std::is_same_v<M, MScale2d> ||
                       std::is_same_v<M, MScale2dXS> ||
                       std::is_same_v<M, MScale2dYS> ||
                       std::is_same_v<M, MScale2dXYS> ) {
            bbox3d->scaleX( _param().x());
            bbox3d->scaleY( _param().y());
            bbox3d->scaleZ( 0.0f );
        }
        if constexpr ( std::is_same_v<M, UITapAreaType> ) {
            type = _param;
        }
//        if constexpr ( std::is_same_v<M, JMATH::Rect2f > ) {
//            bbox = _param;
//        }
        if constexpr ( std::is_same_v<M, UIElementStatus> ) {
            status = _param;
        }
        if constexpr ( std::is_same_v<M, UIFontRef> ) {
            fontRef = _param();
        }
        if constexpr ( std::is_same_v<M, UIFontText> ) {
            fontRef = _param.fontRef;
            text = _param.text;
            fontColor = _param.color;
            fontHeight = _param.height;
        }
        if constexpr ( std::is_same_v<M, UIFontAngle> ) {
            fontAngle = _param();
        }
        if constexpr ( std::is_same_v<M, UIForegroundIcon> ) {
            foreground = _param();
        }
        if constexpr ( std::is_same_v<M, C4f> ) {
            tintColor = _param;
        }
        if constexpr ( std::is_pointer_v<M> ) {
            owner = _param;
        }
    }

public:

    [[nodiscard]] uint64_t Type() const {
        return type();
    }

    [[nodiscard]] std::string Key() const {
        return key;
    }

    [[nodiscard]] Rect2f Area() const {
        return bbox3d->topDown();
    }

    [[nodiscard]] UIElementStatus Status() const {
        return status;
    }

    [[nodiscard]] const ResourceRef& FontRef() const {
        return fontRef;
    }

    [[nodiscard]] const std::string& Text() const {
        return text;
    }

    [[nodiscard]] const std::string& Foreground() const {
        return foreground;
    }

    void Owner( UIView *_elem ) {
        owner = _elem;
    }

    void Font( const Font *_fontPtr ) {
        font = _fontPtr;
    }

    void setVisible( bool _value );
    void toggle();
    void fadeTo( float _duration, float _value );

    void insertGroupElement( UIElementSP _elem );
    [[nodiscard]] bool contains( const V2f& _point ) const;
    [[nodiscard]] bool containsActive( const V2f& _point ) const;
    [[nodiscard]] bool hasActiveStatus() const;
    void touchedDown();
    void touchedUp( const V2f& _point );
    void transform( float _duration, uint64_t _frameSkipper,
                    const V3f& _pos,
                    const Quaternion& _rot = Quaternion{},
                    const V3f& _scale = V3f::ONE );
    void loadResource( std::shared_ptr<Matrix4f> _localHierMat );
    void hoover( bool isHoovering );
    void setStatus( UIElementStatus _status );
    void singleTap();
    void toggleSelected();
private:
    void updateStatus();

private:
    RenderOrchestrator& rsg;

    std::string key;
    UITapAreaType type;
    UIElementStatus status = UIElementStatus::Enabled;
    bool bVisible = true;
    const ::Font *font = nullptr;
    std::string fontRef;
    std::string text;
    float fontAngle = 0.0f;
    float fontHeight = 0.2f;
    C4f fontColor = C4f::WHITE;

    std::string foreground;

    UIView *owner = nullptr;
    std::vector<UIElementSP> groupElements;
    std::string background;
    std::string touchDownAnimNameScale;
    VPListSP foregroundVP;
    VPListSP backgroundVP;
    VPListSP shadowVP;
    MatrixAnim backgroundAnim;
    V4fa backgroundColor;
    C4f defaultBackgroundColor = C4f::WHITE;
    C4f tintColor = C4f::WHITE;
    ControlSingleTapCallback singleTapCallback = ControlTapCallbackEmpty;
    ControlSingleTapCallback singleTapOffToggleCallback = ControlTapCallbackEmpty;
    UICallbackHandle cbParam;
};

enum class UICheckActiveOnly {
    False,
    True
};

class UIContainer2d {
public:
    UIContainer2d( RenderOrchestrator& _rsg,
                   UICallbackMap& _callbackMap,
                   CResourceRef _name ) : rsg( _rsg ), callbackMap(_callbackMap) {
        raii( _name );
    }

    UIContainer2d( RenderOrchestrator& _rsg,
                   UICallbackMap& _callbackMap,
                   CResourceRef _name,
                   UIContainer* _data ) : rsg( _rsg ), callbackMap(_callbackMap) {
        raii( _name );
        unpack( _data );
    }

    void addEmptyCaret();
    void addEmptyCaretNewLine();
    void addTitle( const UIFontText& _text, UIAlignFlags flags = UIAF_None );
    void addListEntry( const ControlDef& _cd );
    void addNavBar( const ControlDef& _logo );
    void addListEntryGrid( const ControlDef& _cd, bool _newLine = false, bool _lastOne = false );
    void addButtonGroupLine( UITapAreaType _uit, const std::vector<ControlDef>& _cds, UIAlignFlags flags = UIAF_None );
    void popCaretX();

    [[nodiscard]] UIElementSP Node() const { return node; };
    void setButtonSize( const MScale2d& _bs );
    void setPadding( const V2f& _value );
    [[nodiscard]] V3f getSize() const;

    void finalize( const MPos2d& _at );
private:
    void raii( CResourceRef _name );
    void unpack( UIContainer* _data );
    void advanceCaret( CSSDisplayMode _displayMode, const MScale2d& _elemSize );
    void addSeparator( float percScaleY = 1.0f );
    void addLabel( const UIFontText& _text, const MScale2d& bsize, CSSDisplayMode displayMode,
                   const V2f& _pos = V2f::ZERO );
    UIElementSP addButton( const ControlDef& _cd, const MScale2d& bisze, CSSDisplayMode displayMode,
                           UITapAreaType _bt = UIT::pushButton, const V2f& _pos = V2f::ZERO );

private:
    RenderOrchestrator& rsg;
    UICallbackMap& callbackMap;

    MPos2d pos;
    V2f padding{ 0.02f, -0.01f };
    V3f size = V3f::ONE;
    UIElementSP node;
    MScale2d bsize{ 0.07f, 0.07f };

    std::unordered_map<std::string, UIElementSP> icontrols;
    float innerPaddedX = 0.0f;
    V2f caret{ V2f::ZERO };
    V2f boundaries{ V2f::ZERO };
    std::vector<V2f> caretQueue;
    std::vector<UIElementSP> wholeLiners;
    MScale2d wholeLineSize{ 0.0f, 0.0f };
};

class UIView {
public:
    UIView( SceneGraph& sg, RenderOrchestrator& rsg, const ColorScheme& cs ) : sg( sg ), rsg( rsg ),
                                                                               colorScheme( cs ) {}

    void add( UIElementSP _elem, UIElementStatus _initialStatus = UIElementStatus::Enabled );
    void add( const MPos2d& _at, UIContainer2d& _container, UIElementStatus _initialStatus = UIElementStatus::Enabled );

    UIElementSP operator()( CResourceRef _key );

    void handleTouchDownEvent( const V2f& _p );
    void handleTouchUpEvent( const V2f& _p );
    void loadResources();
    void hoover( const V2f& _point );
    void updateAnim();

    UIElementSP node( CResourceRef& _key );

    void setButtonStatus( CResourceRef _key, UIElementStatus _status );
    UIElementStatus getButtonStatus( CResourceRef _key ) const;
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

    C4f colorFromStatus( UIElementStatus _status );

    void visit( std::function<void( const UIElementSPConst )> f ) const;
    void foreach( std::function<void( UIElementSP )> f );
    void addCallback( const std::string& _key, UICallbackFunc cf );
    UICallbackMap& Callbacks();
private:
    void clearOnTouchUpEvent();
    void touchDownKeyCached( UIElementSP _key ) const;
    UIElementSP touchDownKeyCached() const;
    void addRecursive( UIElementSP _elem );

private:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    ColorScheme colorScheme;
    std::unordered_map<ResourceRef, UIElementSP> elements;
    UICallbackMap callbacks;
    std::vector<UIElementSP> activeTaps;
    mutable UIElementSP touchDownStartingKey;
};
