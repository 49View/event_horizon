//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <variant>

#include <boost/signals2.hpp>

#include <core/uuid.hpp>
#include <core/command.hpp>
#include <core/suncalc/sun_builder.h>
#include <poly/poly.hpp>
#include <poly/resources/resource_utils.hpp>

class StreamingMediator;
class CameraManager;
class Profile;
class MaterialColor;

//class PolySceneGraphTextureList : public ImageDepencencyMaker {
//    bool addImpl( [[maybe_unused]] ImageBuilder& tbd, [[maybe_unused]] std::unique_ptr<uint8_t []>& _data ) override { return true; };
//};

using NodeGraph = std::unordered_map<std::string, NodeVariants>;
using SceneRayIntersectCallback = std::function<void(const NodeVariants&, float)>;

using NodeGraphConnectParamsSig = NodeVariants&;
using NodeGraphConnectFuncSig = void(NodeGraphConnectParamsSig);

template<typename R> class ResourceBuilder5;

class RawImage;
using ImageBuilder = ResourceBuilder5<RawImage>;
using IB = ImageBuilder;

class Material;
using MaterialBuilder = ResourceBuilder5<Material>;
using MB = MaterialBuilder;

namespace Utility::TTFCore { class Font; }
using FontBuilder = ResourceBuilder5<Utility::TTFCore::Font>;
using FB = FontBuilder;

class SceneGraph;
template<typename T, typename C> class ResourceManager;
class CameraManager;

using ImageManager      = ResourceManager<RawImage, ResourceManagerContainer<RawImage>>;
using FontManager       = ResourceManager<Utility::TTFCore::Font, ResourceManagerContainer<Utility::TTFCore::Font>>;
using ProfileManager    = ResourceManager<Profile, ResourceManagerContainer<Profile>>;
using MaterialManager   = ResourceManager<Material, ResourceManagerContainer<Material>>;
using ColorManager      = ResourceManager<MaterialColor, ResourceManagerContainer<MaterialColor>>;

class CommandScriptSceneGraph : public CommandScript {
public:
    explicit CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() = default;
};

class SceneGraph {
public:
    explicit SceneGraph( CommandQueue& cq,
                         ImageManager& _tl,
                         ProfileManager& _pm,
                         MaterialManager& _ml,
                         ColorManager& _cl,
                         FontManager& _fm,
                         CameraManager& _cm,
                         SunBuilder& _sb );

    void init();
    void add( NodeVariants _geom );
    void remove( const UUID& _uuid );

    void nodeAddConnect( std::function<NodeGraphConnectFuncSig> _slot );

    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdRemoveGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );
    void cmdChangeTime( const std::vector<std::string>& params );

    bool rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;

    ImageManager& TL() { return tl; }
    ProfileManager& PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager& CL() { return cl; }
    CameraManager& CM();
    FontManager& FM() { return fm; }
    ColorManager& MC() { return cl; }
    SunBuilder& SB() { return sb; }

    template <typename R>
    auto& M() {
        if constexpr ( std::is_same<R, RawImage>::value )               return TL();
        if constexpr ( std::is_same<R, Material>::value )               return ML();
        if constexpr ( std::is_same<R, Utility::TTFCore::Font>::value ) return FM();
        if constexpr ( std::is_same<R, Profile>::value )                return PL();
        if constexpr ( std::is_same<R, MaterialColor>::value )          return MC();
    }

    template <typename T>
    T B( const std::string& _name ) {
        return T{ *this, _name };
    }

    void mapGeomType( uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;

    void update();
    NodeGraph& Nodes();

    template <typename T>
    void visitNode( const UUID& _uuid, T _visitor ) {
        if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
            std::visit(_visitor, it->second);
        }
    }

protected:
//    virtual void addImpl( NodeVariants _geom) {};
//    virtual void removeImpl( const UUID& _uuid ) {};
//    virtual void cmdChangeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdRemoveGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    NodeGraph geoms;

    ImageManager& tl;
    ProfileManager& pl;
    MaterialManager& ml;
    ColorManager& cl;
    FontManager& fm;
    CameraManager& cm;

    SunBuilder& sb;

    std::shared_ptr<CommandScriptSceneGraph> hcs;
    std::unordered_map<std::string, uint64_t> geomTypeMap;

    boost::signals2::signal<NodeGraphConnectFuncSig> nodeAddSignal;
};
