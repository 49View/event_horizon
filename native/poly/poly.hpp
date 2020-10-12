//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>
#include <variant>
#include <string>
#include <vector>
#include <core/util_follower.hpp>
#include <core/descriptors/osm_bsdata.hpp>
#include <core/math/poly_shapes.hpp>
#include <core/math/matrix_anim.h>
#include <core/resources/resource_serialization.hpp>
#include <core/names.hpp>
#include <poly/cloth/cloth.h>

struct OSMData;

namespace GTBucket {
    [[maybe_unused]] static constexpr inline int Near = 0;
    [[maybe_unused]] static constexpr inline int NearUnsorted = 1;
    [[maybe_unused]] static constexpr inline int Medium = 2;
    [[maybe_unused]] static constexpr inline int MediumUnsorted = 3;
    [[maybe_unused]] static constexpr inline int Far = 4;
    [[maybe_unused]] static constexpr inline int FarUnsorted = 5;
}

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

    GeomMappingData() = default;
    GeomMappingData(const V2f& _scale) {
        setUVScale(_scale);
    }

    void setUVScale( const V2f& _scale ) {
        uvScale = _scale;
        uvScaleInv = V2fc::ONE / uvScale;
    }

    // Mapping constants
    MappingDirection direction = MappingDirection::X_POS;
    bool bDoNotScaleMapping = false;
    Vector2f offset = V2fc::ZERO;
    MappingMirrorE mirroring = MappingMirrorE::None;
    bool bUnitMapping = false;
    subdivisionAccuray subdivAccuracy = accuracyNone;
    WindingOrderT windingOrder = WindingOrder::CCW;

    JSONSERIALBIN( direction, bDoNotScaleMapping, offset, mirroring, bUnitMapping, subdivAccuracy, windingOrder)
    // Mappping computed
    mutable float fuvScale = 1.0f;
    mutable Vector2f uvScale = V2fc::ONE;
    mutable Vector2f uvScaleInv = V2fc::ONE;
    mutable std::vector<Vector2f> wrapMappingCoords;
    mutable Vector2f pullMappingCoords = V2fc::ZERO;
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

namespace tinygltf{ class Model; }

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
        Quaternion operator()() const noexcept {
            return data;
        }
        Quaternion data;
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

    struct FlipNormal {
        bool constexpr operator()() const noexcept {
            return true;
        }
    };

    struct Z {
        template<typename ...Args>
        explicit Z( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        float operator()() const noexcept {
            return data;
        }
        float data;
    };

    struct Tag {
        template<typename ...Args>
        explicit Tag( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        uint64_t operator()() const noexcept {
            return data;
        }
        uint64_t data;
    };

    struct Bucket {
        template<typename ...Args>
        explicit Bucket( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        int operator()() const noexcept {
            return data;
        }
        int data;
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

    struct Program {
        template<typename ...Args>
        explicit Program( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        ResourceRef operator()() const noexcept {
            return data;
        }
        ResourceRef data;
    };

    struct ForceNormalAxis {
        template<typename ...Args>
        explicit ForceNormalAxis( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V3f operator()() const noexcept {
            return data;
        }
        V3f data;
    };

    struct GLTF2PrimitiveIndex {
        template<typename ...Args>
        explicit GLTF2PrimitiveIndex( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        int operator()() const noexcept {
            return data;
        }
        int data;
    };

    struct GLTF2MeshIndex {
        template<typename ...Args>
        explicit GLTF2MeshIndex( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        int operator()() const noexcept {
            return data;
        }
        int data;
    };

    enum class TextType {
        TextUI,
        Text2d,
        Text3d
    };

    struct GTPolicyColor {
        C4f color;
    };
    struct GTPolicyNameId {
        std::string nameId;
    };
    struct GTPolicyTRS {
        Vector3f pos = V3fc::ZERO;
        Quaternion axis = Quaternion{};
        Vector3f scale = V3fc::ONE;
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
        std::vector<PolyLine2d> sourcePolylines2d;
        std::vector<PolyLine> polyLines;
        Vector3f forcingNormalPoly = V3fc::ZERO;
    };
    struct GTPolicyFollower {
        std::shared_ptr<Profile> profile;
        std::vector<Vector3f> profilePath;
        std::vector<Vector2f> profilePath2d;
        FollowerFlags fflags = FollowerFlags::Defaults;
        PolyRaise fraise = PolyRaise::None;
        Vector2f flipVector = V2fc::ZERO;
        FollowerGap mGaps = FollowerGap::Empty;
        Vector3f mFollowerSuggestedAxis = V3fc::ZERO;
    };
    struct GTPolicyZ {
        float z = 0.0f;
    };
    struct GTPolicyShape {
        ShapeType shapeType = ShapeType::None;
        AABB aabb;
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
    struct GTPolicyGLTF2Model {
        tinygltf::Model* model = nullptr;
        int meshIndex = 0;
        int primitiveIndex = 0;
    };
    struct GTPolicyModifiers {
        bool flipNormals = false;
    };
    struct GTPolicyOSMData {
        OSMData osmData;
        V2f locationLatLon = V2fc::ZERO;
    };

    struct Shape        : GTPolicyTRS, GTPolicyShape, GTPolicyColor {};
    struct Follower     : GTPolicyTRS, GTPolicyFollower, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals, ForceNormalAxis {};
    struct Extrude      : GTPolicyTRS, GTPolicyExtrusion, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals {};
    struct Poly         : GTPolicyTRS, GTPolicyPolyline, GTPolicyMapping, GTPolicyColor, GTPolicyZ, GTReverseNormals {};
    struct Mesh         : GTPolicyTRS, GTPolicyQuad, GTPolicyMapping, GTPolicyColor, GTReverseNormals {};
    struct ClothMesh    : GTPolicyTRS, GTPolicyCloth, GTPolicyMapping, GTPolicyColor, GTReverseNormals {};
    struct GLTF2        : GTPolicyTRS, GTPolicyGLTF2Model, GTPolicyNameId, GTPolicyModifiers {};
    struct Asset        : GTPolicyTRS, GTPolicyNameId {};
    struct OSMTile      : GTPolicyTRS, GTPolicyOSMData {};
    struct OSMBuildings : GTPolicyTRS, GTPolicyOSMData {};
    struct VData        : GTPolicyTRS {};
    struct Text         : GTPolicyTRS, GTPolicyText, GTPolicyColor {};
}
