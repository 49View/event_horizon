//
// Created by Dado on 2019-04-16.
//

#pragma once

#include <memory>
#include <vector>

namespace JMATH {
    class AABB;
}

template <typename T, typename B = JMATH::AABB> class RecursiveTransformation;

class AppData;
class VData;
class Material;
class RawImage;
namespace Utility::TTFCore { class FontInternal;}
using Font = Utility::TTFCore::FontInternal;
struct GeomData;
using Geom = RecursiveTransformation<GeomData>;
using GeomSP = std::shared_ptr<Geom>;
using GeomSPConst = std::shared_ptr<const Geom>;
using GeomSPContainer = std::vector<GeomSP>;
class Profile;
class MaterialColor;
class UIElement;
struct UIContainer;
class CameraRig;

