//
// Created by Dado on 2018-12-28.
//

#pragma once

#include <memory>
#include <variant>

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

namespace GT {
    struct Sphere           {};
    struct Cylinder         {};
    struct Pyramid          {};
    struct Cube             {};
    struct RoundedCube      {};
    struct Panel            {};
    struct Pillow           {};
    struct Arrow            {};
    struct Follower         {};
    struct Extrude          {};
    struct Poly             {};
    struct Mesh             {};
    struct GLTF2            {};
    struct Asset            {};
    struct File             {};
    struct SVG              {};

}