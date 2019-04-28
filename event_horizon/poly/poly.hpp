//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>
#include <variant>
#include <string>
#include <vector>
#include <core/math/poly_shapes.hpp>
#include <core/resources/resource_serialization.hpp>
#include <event_horizon/event_horizon/core/names.hpp>

enum class UIShapeType {
    CameraFrustom2d,
    CameraFrustom3d,
    Line2d,
    Line3d,
    Arrow2d,
    Arrow3d,
    Polygon2d,
    Polygon3d,
    Rect2d,
    Rect3d,
    Text2d,
    Text3d,
    Separator2d,
    Separator3d,
};

enum class RectCreateAnchor {
    None,
    Top,
    Bottom,
    Center,
    Left,
    Right,
    LeftCenter,
    RightCenter
};

enum class RectFillMode {
    Scale,
    AspectFit,
    AspectFill,
    AspectFitLeft,
    AspectFitRight,
    AspectFitTop,
    AspectFitBottom,
};

enum UIRenderFlags {
    NoEffects = 1 << 0,
    DropShaodws = 1 << 1,
    RoundedCorners = 1 << 2
};

enum class GeomBuilderType {
    Sphere,
    Cylinder,
    Pyramid,
    Cube,
    RoundedCube,
    Panel,
    Pillow,
    Arrow,
    Follower,
    Outline,
    Poly,
    Mesh,
    GLTF2,
    Asset,
    File,
    SVG,
};

struct GeomMappingData {
//    MappingDirection getMappingDirection() const { return mapping.direction; }
//    void setMappingDirection( MappingDirection val ) { mapping.direction = val; }
//    Vector2f MappingOffset() const { return mapping.offset; }
//    void MappingOffset( const Vector2f& val ) { mapping.offset = val; }
//    MappingMirrorE MappingMirror() const { return mapping.mirroring; }
//    void MappingMirror( MappingMirrorE val ) { mapping.mirroring = val; }
//    bool UnitMapping() const { return mapping.bUnitMapping; }
//    void UnitMapping( bool val ) { mapping.bUnitMapping = val; }
//    subdivisionAccuray SubdivAccuracy() const { return mSubdivAccuracy; }
//    void SubdivAccuracy( subdivisionAccuray val ) { mSubdivAccuracy = val; }
//    const std::vector<Vector2f>& WrapMappingCoords() const { return wrapMappingCoords; }
//    void WrapMappingCoords( const std::vector<Vector2f>& val ) { wrapMappingCoords = val; }

    // Mapping constants
    MappingDirection direction = MappingDirection::X_POS;
    bool bDoNotScaleMapping = false;
    Vector2f offset = Vector2f::ZERO;
    MappingMirrorE mirroring = MappingMirrorE::None;
    bool bUnitMapping = false;
    subdivisionAccuray subdivAccuracy = accuracyNone;
    WindingOrderT windingOrder = WindingOrder::CCW;

    JSONSERIALBIN( direction, bDoNotScaleMapping, offset, mirroring, bUnitMapping, subdivAccuracy, windingOrder)
    // Mappping computed
    mutable float fuvScale = 1.0f;
    mutable Vector2f uvScale = Vector2f::ONE;
    mutable Vector2f uvScaleInv = Vector2f::ONE;
    mutable std::vector<Vector2f> wrapMappingCoords;
    mutable Vector2f pullMappingCoords = Vector2f::ZERO;
};

struct PolyOutLine;
struct PolyLine;
namespace Utility::TTFCore { class FontInternal;}
using Font = Utility::TTFCore::FontInternal;

namespace GT {
    enum class TextType {
        TextUI,
        Text2d,
        Text3d
    };

    struct GTPolicyColor {
        C4f color;
    };
    struct GTPolicyText {
        std::string text;
        std::string fontName = S::DEFAULT_FONT;
        std::shared_ptr<Font> font;
        TextType ttype;
        float fontHeight = .1f;
        JSONSERIALBIN( text, fontName, ttype, fontHeight )
    };
    struct GTPolicyExtrusion {
        std::vector<PolyOutLine> extrusionVerts;
    };
    struct GTPolicyPolyline {
        std::vector<Vector3f> sourcePolysVList;
        std::vector<Triangle3d> sourcePolysTris;
        std::vector<PolyLine> polyLines;
        Vector3f forcingNormalPoly = Vector3f::ZERO;
        ReverseFlag rfPoly = ReverseFlag::False;
    };
    struct GTPolicyShape {
        ShapeType shapeType = ShapeType::None;
    };
    struct GTPolicyMapping {
        GeomMappingData mappingData;
    };

    struct Shape   : GTPolicyShape, GTPolicyColor {};
    struct Follower         {};
    struct Extrude : GTPolicyExtrusion, GTPolicyMapping, GTPolicyColor {};
    struct Poly    : GTPolicyPolyline, GTPolicyMapping, GTPolicyColor {};
    struct Mesh             {};
    struct GLTF2            {};
    struct Asset            {};
    struct File             {};
    struct SVG              {};
    struct Text : GTPolicyText, GTPolicyColor {};
}