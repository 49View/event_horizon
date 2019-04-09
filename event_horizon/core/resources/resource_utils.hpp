//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <tuple>
#include <unordered_map>

#include <core/serialization.hpp>

class VData;
class Material;
class RawImage;
namespace Utility::TTFCore { class Font; }
class GeomData;
class Profile;
class MaterialColor;
class UIElement;
class CameraRig;


enum class AddResourcePolicy {
    Immediate,
    Deferred
};

JSONDATA( ResourceTarDict, group, filename, hash )
    ResourceTarDict( std::string group, std::string filename, std::string hash ) :
            group( std::move( group )), filename( std::move( filename )), hash( std::move( hash )) {}

    std::string group;
    std::string filename;
    std::string hash;
};

struct ResourcePipeElement {
    std::string name;
    std::string hash;
    std::string metadata;
};

template <typename T>
struct ResourceTransfer {
    std::shared_ptr<T>      elem;
    std::string             hash;
    StringUniqueCollection  names;

    // operator < is needed for boost signal sorting
    bool operator <(const ResourceTransfer &b) const {
        return hash < b.hash;
    }
};

template <typename C>
using ResourceManagerContainer = std::unordered_map<std::string,std::shared_ptr<C>>;

template <typename T>
using SignalsDeferredContainer = std::set<ResourceTransfer<T>>;

inline static size_t resourcePriority( const ResourceRef& ref ) {
    if ( ref == ResourceGroup::Image ||
         ref == ResourceGroup::VData ||
         ref == ResourceGroup::Font ||
         ref == ResourceGroup::Color ||
         ref == ResourceGroup::CameraRig ||
         ref == ResourceGroup::Profile ) return 0;

    if ( ref == ResourceGroup::Material ) return 10;

    if ( ref == ResourceGroup::Geom || ref == ResourceGroup::UI ) return 20;

        return 0;
}

template <typename R>
class ResourceVersioning {
public:

    inline static size_t Version() {
        if ( std::is_same<R, Material>::value )         return 2500;
        if ( std::is_same<R, GeomData>::value )         return 2000;
        if ( std::is_same<R, VData>::value )            return 1000;
        if ( std::is_same<R, MaterialColor>::value  )   return 1000;
        return 0;
    }

    inline static bool HasDeps() {
        if constexpr ( std::is_same<R, VData>::value )                  return false;
        if constexpr ( std::is_same<R, Material>::value )               return true ;
        if constexpr ( std::is_same<R, GeomData>::value )               return true ;
        if constexpr ( std::is_same<R, MaterialColor>::value )          return false;
        if constexpr ( std::is_same<R, Profile>::value )                return false;
        if constexpr ( std::is_same<R, RawImage>::value )               return false;
        if constexpr ( std::is_same<R, Utility::TTFCore::Font>::value ) return false;
        if constexpr ( std::is_same<R, UIElement>::value )              return true ;
        if constexpr ( std::is_same<R, CameraRig>::value )              return false;
    }

    inline static std::string Prefix() {
        if constexpr ( std::is_same<R, VData>::value ) return ResourceGroup::VData;
        if constexpr ( std::is_same<R, Material>::value ) return ResourceGroup::Material;
        if constexpr ( std::is_same<R, GeomData>::value ) return ResourceGroup::Geom;
        if constexpr ( std::is_same<R, MaterialColor>::value ) return ResourceGroup::Color;
        if constexpr ( std::is_same<R, Profile>::value ) return ResourceGroup::Profile;
        if constexpr ( std::is_same<R, RawImage>::value ) return ResourceGroup::Image;
        if constexpr ( std::is_same<R, Utility::TTFCore::Font>::value ) return ResourceGroup::Font;
        if constexpr ( std::is_same<R, UIElement>::value ) return ResourceGroup::UI;
        if constexpr ( std::is_same<R, CameraRig>::value ) return ResourceGroup::CameraRig;
    }

    inline static std::string GenerateThumbnail( const R& _res ) {
        if ( std::is_same<R, VData>::value    )                 return "vdata";
        if ( std::is_same<R, Material>::value )                 return "material";
        if ( std::is_same<R, GeomData>::value )                 return "geom";

        if ( std::is_same<R, MaterialColor>::value  )           return "color";
        if ( std::is_same<R, CameraRig>::value )                return "camera";

        if ( std::is_same<R, Profile>::value  )                 return "profile";
        if ( std::is_same<R, RawImage>::value  )                return "image";
        if ( std::is_same<R, Utility::TTFCore::Font>::value  )  return "font";
        return "unknown";
    }

};

template<typename R> class ResourceBuilder5;

using VDataBuilder = ResourceBuilder5<VData>;
using VB = VDataBuilder;

using ImageBuilder = ResourceBuilder5<RawImage>;
using IB = ImageBuilder;

using ProfileBuilder = ResourceBuilder5<Profile>;
using PB = ProfileBuilder;

using CameraBuilder = ResourceBuilder5<CameraRig>;
using CB = CameraBuilder;

using FontBuilder = ResourceBuilder5<Utility::TTFCore::Font>;
using FB = FontBuilder;

using MaterialColorBuilder = ResourceBuilder5<MaterialColor>;
using MCB = MaterialColorBuilder;

using MaterialBuilder = ResourceBuilder5<Material>;
using MB = MaterialBuilder;

//using GeomBuilder = ResourceBuilder5<GeomData>;
//using GDB = GeomBuilder;

template<typename T, typename C> class ResourceManager;

using VDataManager      = ResourceManager<VData, ResourceManagerContainer<VData>>;
using ImageManager      = ResourceManager<RawImage, ResourceManagerContainer<RawImage>>;
using FontManager       = ResourceManager<Utility::TTFCore::Font, ResourceManagerContainer<Utility::TTFCore::Font>>;
using ProfileManager    = ResourceManager<Profile, ResourceManagerContainer<Profile>>;
using MaterialManager   = ResourceManager<Material, ResourceManagerContainer<Material>>;
using ColorManager      = ResourceManager<MaterialColor, ResourceManagerContainer<MaterialColor>>;
using CameraManager     = ResourceManager<CameraRig, ResourceManagerContainer<CameraRig>>;
using GeomManager       = ResourceManager<GeomData, ResourceManagerContainer<GeomData>>;
