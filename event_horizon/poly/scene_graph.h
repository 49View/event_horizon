#include <utility>

#include <utility>

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

#include <core/http/webclient.h>
#include <core/uuid.hpp>
#include <core/command.hpp>
#include <core/recursive_transformation.hpp>
#include <core/geom.hpp>
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <poly/poly.hpp>
#include <poly/node_graph.hpp>
#include <poly/vdata_assembler.h>

class SceneGraph;

class CommandScriptSceneGraph : public CommandScript {
public:
    explicit CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() = default;
};

struct LoadedResouceCallbackData {
    LoadedResouceCallbackData( ResourceRef  key, SerializableContainer&& data,
                               HttpDeferredResouceCallbackFunction  ccf ) :
                               key(std::move( key )), data( std::move(data) ), ccf(std::move( ccf )) {}

    ResourceRef                         key;
    SerializableContainer               data;
    HttpDeferredResouceCallbackFunction ccf;
};

using LoadedResouceCallbackContainer = std::vector<LoadedResouceCallbackData>;
using GenericSceneCallbackValueMap = std::tuple<std::string, SerializableContainer>;
using GenericSceneCallback = std::unordered_map<std::string, GenericSceneCallbackValueMap>;

class SceneGraph : public NodeGraph {
public:
    explicit SceneGraph( CommandQueue& cq,
                         VDataManager& _vl,
                         ImageManager& _tl,
                         ProfileManager& _pm,
                         MaterialManager& _ml,
                         ColorManager& _cl,
                         FontManager& _fm,
                         CameraManager& _cm,
                         GeomManager& _gm);

    void init();
    UUID addNode( const ResourceRef& _hash );
    void removeNode( const UUID& _uuid );

    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdRemoveGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );
    void cmdChangeTime( const std::vector<std::string>& params );
    void cmdReloadShaders( const std::vector<std::string>& _params );

    bool rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;

    ImageManager&    TL() { return tl; }
    VDataManager&    VL() { return vl; }
    ProfileManager&  PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager&    CL() { return cl; }
    CameraManager&   CM() { return cm; }
    FontManager&     FM() { return fm; }
    ColorManager&    MC() { return cl; }
    GeomManager&     GM() { return gm; }

    std::shared_ptr<VData        >  VL( const ResourceRef& _ref ) const { return vl.get(_ref); }
    std::shared_ptr<RawImage     >  TL( const ResourceRef& _ref ) const { return tl.get(_ref); }
    std::shared_ptr<Material     >  ML( const ResourceRef& _ref ) const { return ml.get(_ref); }
    std::shared_ptr<Font         >  FM( const ResourceRef& _ref ) const { return fm.get(_ref); }
    std::shared_ptr<Profile      >  PL( const ResourceRef& _ref ) const { return pl.get(_ref); }
    std::shared_ptr<MaterialColor>  CL( const ResourceRef& _ref ) const { return cl.get(_ref); }
    std::shared_ptr<CameraRig    >  CM( const ResourceRef& _ref ) const { return cm.get(_ref); }
    std::shared_ptr<Geom         >  GM( const ResourceRef& _ref ) const { return gm.get(_ref); }
    std::shared_ptr<MaterialColor>  MC( const ResourceRef& _ref ) const { return cl.get(_ref); }

    template <typename T>
    std::shared_ptr<T> get( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref); }
    }

    template <typename R>
    auto& M() {
        if constexpr ( std::is_same_v<R, VData>         ) return VL();
        if constexpr ( std::is_same_v<R, RawImage>      ) return TL();
        if constexpr ( std::is_same_v<R, Material>      ) return ML();
        if constexpr ( std::is_same_v<R, Font>          ) return FM();
        if constexpr ( std::is_same_v<R, Profile>       ) return PL();
        if constexpr ( std::is_same_v<R, MaterialColor> ) return MC();
        if constexpr ( std::is_same_v<R, CameraRig>     ) return CM();
        if constexpr ( std::is_same_v<R, Geom>          ) return GM();
    }

    template <typename R>
    static void addDeferred( const ResourceRef& _key, SerializableContainer&& _res, HttpDeferredResouceCallbackFunction _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) resourceCallbackVData        .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) resourceCallbackRawImage     .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) resourceCallbackMaterial     .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) resourceCallbackFont         .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) resourceCallbackProfile      .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) resourceCallbackMaterialColor.emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) resourceCallbackCameraRig    .emplace_back( _key, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) resourceCallbackGeom         .emplace_back( _key, std::move(_res), _ccf );
    }
    static void addDeferredComp( SerializableContainer&& _data, HttpDeferredResouceCallbackFunction _ccf = nullptr ) {
        resourceCallbackComposite.emplace_back( "", std::move(_data), _ccf );
    }

    template <typename R>
    ResourceRef add( const ResourceRef& _key, const R& _res, HttpDeferredResouceCallbackFunction _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) return addVData        ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) return addRawImage     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) return addMaterial     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) return addFont         ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) return addProfile      ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) return addMaterialColor( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) return addCameraRig    ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) return addGeom         ( _key, _res, _ccf );
    }

    ResourceRef addVData         ( const ResourceRef& _key, const VData        & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addRawImage      ( const ResourceRef& _key, const RawImage     & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addMaterial      ( const ResourceRef& _key, const Material     & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addFont          ( const ResourceRef& _key, const Font         & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addProfile       ( const ResourceRef& _key, const Profile      & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addMaterialColor ( const ResourceRef& _key, const MaterialColor& _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addCameraRig     ( const ResourceRef& _key, const CameraRig    & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    ResourceRef addGeom          ( const ResourceRef& _key, const Geom         & _res, HttpDeferredResouceCallbackFunction _ccf = nullptr );
    void addResources( const SerializableContainer& _data, HttpDeferredResouceCallbackFunction _ccf = nullptr );

    static void addGenericCallback( const std::string& _key, GenericSceneCallbackValueMap&& _value ) {
        SceneGraph::genericSceneCallback.emplace( _key, std::move(_value) );
    }

    template <typename T>
    T B( const std::string& _name ) {
        return T{ *this, _name };
    }

    void mapGeomType( uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;

    void update();
    NodeGraphContainer& Nodes();

    template <typename T>
    void visitNode( const UUID& _uuid, T _visitor ) {
        if ( auto it = nodes.find(_uuid); it != nodes.end() ) {
            std::visit(_visitor, it->second);
        }
    }

    template <typename T, typename ...Args>
    UUID GB( Args&&... args ) {
        VDataAssembler<T> gb{std::forward<Args>(args)...};
        auto matRef     = ML().getHash(gb.matRef);

        VDataServices::prepare( *this, gb.dataTypeHolder );
        auto hashRefName = VDataServices::refName( gb.dataTypeHolder );
        auto vdataRef = VL().getHash( hashRefName );
        if ( vdataRef.empty() ) {
            vdataRef = B<VB>( hashRefName ).addIM( VDataServices::build(gb.dataTypeHolder) );
        }

        auto elem = std::make_shared<Geom>(gb.Name());
        elem->pushData( vdataRef, matRef );

        if ( gb.elemInjFather ) gb.elemInjFather->addChildren(elem);
        elem->updateExistingTransform( gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale );
        auto ref = B<GRB>( gb.Name() ).addIM( elem );
        return gb.elemInjFather ? ref : addNode( ref );
    }

    GeomSP GC();
    UUID GC( const GeomSP& _geom );

    static GenericSceneCallback           genericSceneCallback         ;
    static LoadedResouceCallbackContainer resourceCallbackVData        ;
    static LoadedResouceCallbackContainer resourceCallbackRawImage     ;
    static LoadedResouceCallbackContainer resourceCallbackMaterial     ;
    static LoadedResouceCallbackContainer resourceCallbackFont         ;
    static LoadedResouceCallbackContainer resourceCallbackProfile      ;
    static LoadedResouceCallbackContainer resourceCallbackMaterialColor;
    static LoadedResouceCallbackContainer resourceCallbackCameraRig    ;
    static LoadedResouceCallbackContainer resourceCallbackGeom         ;
    static LoadedResouceCallbackContainer resourceCallbackComposite    ;

protected:
//    virtual void cmdChangeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdRemoveGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    VDataManager& vl;
    ImageManager& tl;
    ProfileManager& pl;
    MaterialManager& ml;
    ColorManager& cl;
    FontManager& fm;
    CameraManager& cm;
    GeomManager& gm;

    std::shared_ptr<CommandScriptSceneGraph> hcs;
};
