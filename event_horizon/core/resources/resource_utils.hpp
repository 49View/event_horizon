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
    HttpResouceCB ccf = nullptr;

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
    if ( ref == ResourceGroup::AppData ) return 0;

    if ( ref == ResourceGroup::Image ||
         ref == ResourceGroup::VData ||
         ref == ResourceGroup::Font ||
         ref == ResourceGroup::Color ||
         ref == ResourceGroup::CameraRig ||
         ref == ResourceGroup::Light ||
         ref == ResourceGroup::Profile ) return 1;

    if ( ref == ResourceGroup::Material ) return 10;

    if ( ref == ResourceGroup::Geom || ref == ResourceGroup::UI ) return 20;

        return 0;
}

template <typename R>
class ResourceVersioning {
public:

    inline static size_t Version() {
        if ( std::is_same<R, AppData>::value )          return  1000;
        if ( std::is_same<R, Material>::value )         return 2500;
        if ( std::is_same<R, Geom>::value )             return 2000;
        if ( std::is_same<R, VData>::value )            return 1000;
        if ( std::is_same<R, MaterialColor>::value  )   return 1000;
        if ( std::is_same<R, Light>::value  )           return 1000;
        return 0;
    }

    inline static bool HasDeps() {
        if constexpr ( std::is_same<R, AppData>::value )                return false;
        if constexpr ( std::is_same<R, VData>::value )                  return false;
        if constexpr ( std::is_same<R, Material>::value )               return true ;
        if constexpr ( std::is_same<R, Geom>::value )                   return true ;
        if constexpr ( std::is_same<R, MaterialColor>::value )          return false;
        if constexpr ( std::is_same<R, Profile>::value )                return false;
        if constexpr ( std::is_same<R, RawImage>::value )               return false;
        if constexpr ( std::is_same<R, Font>::value )                   return false;
        if constexpr ( std::is_same<R, UIContainer>::value )            return true ;
        if constexpr ( std::is_same<R, CameraRig>::value )              return false;
        if constexpr ( std::is_same<R, Light>::value )                  return false;
    }

    inline static SerializableContainer HashResolver( std::shared_ptr<R> _val ) {
        if constexpr ( std::is_same<R, AppData>::value )                return _val->serialize();
        if constexpr ( std::is_same<R, VData>::value )                  return _val->serialize();
        if constexpr ( std::is_same<R, Material>::value )               return _val->serialize();
        if constexpr ( std::is_same<R, Geom>::value )                   return _val->serialize();
        if constexpr ( std::is_same<R, MaterialColor>::value )          return {};
        if constexpr ( std::is_same<R, Profile>::value )                return _val->serialize();
        if constexpr ( std::is_same<R, RawImage>::value )               return _val->serialize();
        if constexpr ( std::is_same<R, Light>::value )                  return _val->serialize();
        if constexpr ( std::is_same<R, Font>::value )                   return {};
        if constexpr ( std::is_same<R, UIContainer>::value )            return serializableContainerFromString(_val->serialize());
        if constexpr ( std::is_same<R, CameraRig>::value )              return {};
    }

    inline static std::string Prefix() {
        if constexpr ( std::is_same<R, AppData>::value ) return ResourceGroup::AppData;
        if constexpr ( std::is_same<R, VData>::value ) return ResourceGroup::VData;
        if constexpr ( std::is_same<R, Material>::value ) return ResourceGroup::Material;
        if constexpr ( std::is_same<R, Geom>::value ) return ResourceGroup::Geom;
        if constexpr ( std::is_same<R, MaterialColor>::value ) return ResourceGroup::Color;
        if constexpr ( std::is_same<R, Profile>::value ) return ResourceGroup::Profile;
        if constexpr ( std::is_same<R, RawImage>::value ) return ResourceGroup::Image;
        if constexpr ( std::is_same<R, Font>::value ) return ResourceGroup::Font;
        if constexpr ( std::is_same<R, UIContainer>::value ) return ResourceGroup::UI;
        if constexpr ( std::is_same<R, CameraRig>::value ) return ResourceGroup::CameraRig;
        if constexpr ( std::is_same<R, Light>::value ) return ResourceGroup::Light;
    }

    inline static std::string GenerateThumbnail( const R& _res ) {
        if ( std::is_same<R, AppData>::value    )               return "appData";
        if ( std::is_same<R, VData>::value    )                 return "vdata";
        if ( std::is_same<R, Material>::value )                 return "material";
        if ( std::is_same<R, Geom>::value )                     return "geom";

        if ( std::is_same<R, MaterialColor>::value  )           return "color";
        if ( std::is_same<R, CameraRig>::value )                return "camera";

        if ( std::is_same<R, Profile>::value  )                 return "profile";
        if ( std::is_same<R, RawImage>::value  )                return "image";
        if ( std::is_same<R, Light>::value  )                   return "light";
        if ( std::is_same<R, Font>::value  )                    return "font";
        return "unknown";
    }

};


template<typename R, typename SG, template<typename T> typename RV> class ResourceBuilder;

class SceneGraph;

using AppDataBuilder = ResourceBuilder<AppData, SceneGraph, ResourceVersioning>;
using AB = AppDataBuilder;

using VDataBuilder = ResourceBuilder<VData, SceneGraph, ResourceVersioning>;
using VB = VDataBuilder;

using ImageBuilder = ResourceBuilder<RawImage, SceneGraph, ResourceVersioning>;
using IB = ImageBuilder;

using ProfileBuilder = ResourceBuilder<Profile, SceneGraph, ResourceVersioning>;
using PB = ProfileBuilder;

using CameraBuilder = ResourceBuilder<CameraRig, SceneGraph, ResourceVersioning>;
using CB = CameraBuilder;

using FontBuilder = ResourceBuilder<Font, SceneGraph, ResourceVersioning>;
using FB = FontBuilder;

using MaterialColorBuilder = ResourceBuilder<MaterialColor, SceneGraph, ResourceVersioning>;
using MCB = MaterialColorBuilder;

using MaterialBuilder = ResourceBuilder<Material, SceneGraph, ResourceVersioning>;
using MB = MaterialBuilder;

using LightBuilder = ResourceBuilder<Light, SceneGraph, ResourceVersioning>;
using LB = LightBuilder;

using UIBuilder = ResourceBuilder<UIContainer, SceneGraph, ResourceVersioning>;
using UIB = UIBuilder;

using GeomRBuilder = ResourceBuilder<Geom, SceneGraph, ResourceVersioning>;
using GRB = GeomRBuilder;

template<typename T, typename C> class ResourceManager;

using AppDataManager    = ResourceManager<AppData, ResourceManagerContainer<AppData>>;
using VDataManager      = ResourceManager<VData, ResourceManagerContainer<VData>>;
using ImageManager      = ResourceManager<RawImage, ResourceManagerContainer<RawImage>>;
using FontManager       = ResourceManager<Font, ResourceManagerContainer<Font>>;
using ProfileManager    = ResourceManager<Profile, ResourceManagerContainer<Profile>>;
using MaterialManager   = ResourceManager<Material, ResourceManagerContainer<Material>>;
using ColorManager      = ResourceManager<MaterialColor, ResourceManagerContainer<MaterialColor>>;
using CameraManager     = ResourceManager<CameraRig, ResourceManagerContainer<CameraRig>>;
using GeomManager       = ResourceManager<Geom, ResourceManagerContainer<Geom>>;
using UIManager         = ResourceManager<UIContainer, ResourceManagerContainer<UIContainer>>;
using LightManager      = ResourceManager<Light, ResourceManagerContainer<Light>>;

struct LoadedResouceCallbackData {
    LoadedResouceCallbackData( ResourceRef  key, ResourceRef _hash, SerializableContainer&& data,
                               HttpResouceCB  ccf ) :
            key(std::move( key )), hash(std::move( _hash )),
            data( std::move(data) ), ccf(std::move( ccf )) {}

    ResourceRef                         key;
    ResourceRef                         hash;
    SerializableContainer               data;
    HttpResouceCB ccf;
};

using LoadedResouceCallbackContainer = std::vector<LoadedResouceCallbackData>;
