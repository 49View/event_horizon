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
#include <poly/resources/resource_utils.hpp>
#include <core/suncalc/sun_builder.h>
#include <poly/poly.hpp>

class StreamingMediator;
class CameraManager;
class RawImage;

//class PolySceneGraphTextureList : public ImageDepencencyMaker {
//    bool addImpl( [[maybe_unused]] ImageBuilder& tbd, [[maybe_unused]] std::unique_ptr<uint8_t []>& _data ) override { return true; };
//};

using NodeGraph = std::unordered_map<std::string, NodeVariants>;
using SceneRayIntersectCallback = std::function<void(const NodeVariants&, float)>;

using NodeGraphConnectParamsSig = NodeVariants&;
using NodeGraphConnectFuncSig = void(NodeGraphConnectParamsSig);

class SceneGraph;
class ProfileManager;
template<typename T, typename C> class ResourceManager;
class MaterialManager;
class ColorManager;
class FontManager;
class CameraManager;

using ImageManager = ResourceManager<RawImage, ResourceManagerContainer<RawImage>>;

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

//class PolySceneGraph : public SceneGraph {
//public:
//    PolySceneGraph(CommandQueue& cq, FontManager& _fm, SunBuilder& _sb, CameraManager& _cm) : SceneGraph(cq, _fm, _sb, _cm) {
//        ml.TL(&tl);
//    }
//
//    DependencyMaker& TL() override { return tl; }
//private:
//    PolySceneGraphTextureList tl;
//};
