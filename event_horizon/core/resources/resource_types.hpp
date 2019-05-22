//
// Created by Dado on 2019-04-16.
//

#pragma once

#include <memory>
#include <vector>

template <typename T> class RecursiveTransformation;

class VData;
class Material;
class RawImage;
namespace Utility::TTFCore { class FontInternal;}
using Font = Utility::TTFCore::FontInternal;
struct GeomData;
using Geom = RecursiveTransformation<GeomData>;
using GeomSP = std::shared_ptr<Geom>;
using GeomSPContainer = std::vector<GeomSP>;
class Profile;
class MaterialColor;
class UIElement;
class CameraRig;

