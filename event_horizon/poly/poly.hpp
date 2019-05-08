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
#include <core/names.hpp>
#include <poly/cloth/cloth.h>

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

struct QuadVector3fNormal {
    QuadVector3f quad;
    Vector3f normal;
};

struct PolyOutLine;
struct PolyLine;
struct PolyLine2d;
class Profile;
namespace Utility::TTFCore { class FontInternal;}
using Font = Utility::TTFCore::FontInternal;
using QuadVector3fNormalfList = std::vector<QuadVector3fNormal>;

namespace GT {

    struct Scale {
        template<typename ...Args>
        explicit Scale( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V3f operator()() const noexcept {
            return data;
        }
        V3f data;
    };

    struct Rotate {
        template<typename ...Args>
        explicit Rotate( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V4f operator()() const noexcept {
            return data;
        }
        V4f data;
    };

    struct Direction {
        template<typename ...Args>
        explicit Direction( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V3f operator()() const noexcept {
            return data;
        }
        V3f data;
    };

    struct Flip {
        template<typename ...Args>
        explicit Flip( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V2f operator()() const noexcept {
            return data;
        }
        V2f data;
    };

    struct Z {
        template<typename ...Args>
        explicit Z( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        float operator()() const noexcept {
            return data;
        }
        float data;
    };

    struct A {
        template<typename ...Args>
        explicit A( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        float operator()() const noexcept {
            return data;
        }
        float data;
    };

    struct M {
        template<typename ...Args>
        explicit M( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        ResourceRef operator()() const noexcept {
            return data;
        }
        ResourceRef data;
    };

    enum class TextType {
        TextUI,
        Text2d,
        Text3d
    };

    struct GTPolicyColor {
        C4f color;
    };
    struct GTPolicyTRS {
        Vector3f pos = Vector3f::ZERO;
        Vector4f axis = Vector4f::QUAT_UNIT;
        Vector3f scale = Vector3f::ONE;
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
    struct GTReverseNormals {
        ReverseFlag rfPoly = ReverseFlag::False;
    };
    struct GTPolicyPolyline {
        std::vector<Vector3f> sourcePolysVList;
        std::vector<Triangle3d> sourcePolysTris;
        std::vector<PolyLine> polyLines;
        Vector3f forcingNormalPoly = Vector3f::ZERO;
    };
    struct GTPolicyFollower {
        std::shared_ptr<Profile> profile;
        std::vector<Vector3f> profilePath;
        std::vector<Vector2f> profilePath2d;
        FollowerFlags fflags = FollowerFlags::Defaults;
        PolyRaise fraise = PolyRaise::None;
        Vector2f flipVector = Vector2f::ZERO;
        FollowerGap mGaps = FollowerGap::Empty;
        Vector3f mFollowerSuggestedAxis = Vector3f::ZERO;
    };
    struct GTPolicyZ {
        float z = 0.0f;
    };
    struct GTPolicyShape {
        ShapeType shapeType = ShapeType::None;
    };
    struct GTPolicyMapping {
        GeomMappingData mappingData;
    };
    struct GTPolicyQuad {
        QuadVector3fNormalfList quads;
    };
    struct GTPolicyCloth {
        std::shared_ptr<Cloth>  cloth;
    };

    struct Shape     : GTPolicyTRS, GTPolicyShape, GTPolicyColor {};
    struct Follower  : GTPolicyTRS, GTPolicyFollower, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals {};
    struct Extrude   : GTPolicyTRS, GTPolicyExtrusion, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals {};
    struct Poly      : GTPolicyTRS, GTPolicyPolyline, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals {};
    struct Mesh      : GTPolicyTRS, GTPolicyQuad, GTPolicyMapping, GTPolicyColor {};
    struct ClothMesh : GTPolicyTRS, GTPolicyCloth, GTPolicyMapping, GTPolicyColor {};
    struct GLTF2     {};
    struct Asset     {};
    struct File      {};
    struct SVG       {};
    struct Text      : GTPolicyTRS, GTPolicyText, GTPolicyColor {};
}
