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
#include <core/http/webclient.h>
#include <core/resources/resource_types.hpp>

enum class AddResourcePolicy {
    Immediate,
    Deferred
};

using CommandResouceCallbackFunction = std::function<void(const std::vector<std::string>&)>;

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
    HttpDeferredResouceCallbackFunction ccf = nullptr;

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
        if ( std::is_same<R, Geom>::value )         return 2000;
        if ( std::is_same<R, VData>::value )            return 1000;
        if ( std::is_same<R, MaterialColor>::value  )   return 1000;
        return 0;
    }

    inline static bool HasDeps() {
        if constexpr ( std::is_same<R, VData>::value )                  return false;
        if constexpr ( std::is_same<R, Material>::value )               return true ;
        if constexpr ( std::is_same<R, Geom>::value )                   return true ;
        if constexpr ( std::is_same<R, MaterialColor>::value )          return false;
        if constexpr ( std::is_same<R, Profile>::value )                return false;
        if constexpr ( std::is_same<R, RawImage>::value )               return false;
        if constexpr ( std::is_same<R, Font>::value ) return false;
        if constexpr ( std::is_same<R, UIElement>::value )              return true ;
        if constexpr ( std::is_same<R, CameraRig>::value )              return false;
    }

    inline static SerializableContainer HashResolver( std::shared_ptr<R> _val ) {
        if constexpr ( std::is_same<R, VData>::value )                  return _val->serialize();
        if constexpr ( std::is_same<R, Material>::value )               return _val->serialize();
        if constexpr ( std::is_same<R, Geom>::value )                   return _val->serialize();
        if constexpr ( std::is_same<R, MaterialColor>::value )          return {};
        if constexpr ( std::is_same<R, Profile>::value )                return _val->serialize();
        if constexpr ( std::is_same<R, RawImage>::value )               return _val->serialize();
        if constexpr ( std::is_same<R, Font>::value ) return {};
        if constexpr ( std::is_same<R, UIElement>::value )              return {};
        if constexpr ( std::is_same<R, CameraRig>::value )              return {};
    }

    inline static std::string Prefix() {
        if constexpr ( std::is_same<R, VData>::value ) return ResourceGroup::VData;
        if constexpr ( std::is_same<R, Material>::value ) return ResourceGroup::Material;
        if constexpr ( std::is_same<R, Geom>::value ) return ResourceGroup::Geom;
        if constexpr ( std::is_same<R, MaterialColor>::value ) return ResourceGroup::Color;
        if constexpr ( std::is_same<R, Profile>::value ) return ResourceGroup::Profile;
        if constexpr ( std::is_same<R, RawImage>::value ) return ResourceGroup::Image;
        if constexpr ( std::is_same<R, Font>::value ) return ResourceGroup::Font;
        if constexpr ( std::is_same<R, UIElement>::value ) return ResourceGroup::UI;
        if constexpr ( std::is_same<R, CameraRig>::value ) return ResourceGroup::CameraRig;
    }

    inline static std::string GenerateThumbnail( const R& _res ) {
        if ( std::is_same<R, VData>::value    )                 return "vdata";
        if ( std::is_same<R, Material>::value )                 return "material";
        if ( std::is_same<R, Geom>::value )                 return "geom";

        if ( std::is_same<R, MaterialColor>::value  )           return "color";
        if ( std::is_same<R, CameraRig>::value )                return "camera";

        if ( std::is_same<R, Profile>::value  )                 return "profile";
        if ( std::is_same<R, RawImage>::value  )                return "image";
        if ( std::is_same<R, Font>::value  )  return "font";
        return "unknown";
    }

};

template<typename R> class ResourceBuilder;

using VDataBuilder = ResourceBuilder<VData>;
using VB = VDataBuilder;

using ImageBuilder = ResourceBuilder<RawImage>;
using IB = ImageBuilder;

using ProfileBuilder = ResourceBuilder<Profile>;
using PB = ProfileBuilder;

using CameraBuilder = ResourceBuilder<CameraRig>;
using CB = CameraBuilder;

using FontBuilder = ResourceBuilder<Font>;
using FB = FontBuilder;

using MaterialColorBuilder = ResourceBuilder<MaterialColor>;
using MCB = MaterialColorBuilder;

using MaterialBuilder = ResourceBuilder<Material>;
using MB = MaterialBuilder;

using GeomRBuilder = ResourceBuilder<Geom>;
using GRB = GeomRBuilder;

template<typename T, typename C> class ResourceManager;

using VDataManager      = ResourceManager<VData, ResourceManagerContainer<VData>>;
using ImageManager      = ResourceManager<RawImage, ResourceManagerContainer<RawImage>>;
using FontManager       = ResourceManager<Font, ResourceManagerContainer<Font>>;
using ProfileManager    = ResourceManager<Profile, ResourceManagerContainer<Profile>>;
using MaterialManager   = ResourceManager<Material, ResourceManagerContainer<Material>>;
using ColorManager      = ResourceManager<MaterialColor, ResourceManagerContainer<MaterialColor>>;
using CameraManager     = ResourceManager<CameraRig, ResourceManagerContainer<CameraRig>>;
using GeomManager       = ResourceManager<Geom, ResourceManagerContainer<Geom>>;
