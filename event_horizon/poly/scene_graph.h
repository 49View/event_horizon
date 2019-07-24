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
struct scene_t;
class Camera;

class CommandScriptSceneGraph : public CommandScript {
public:
    explicit CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() = default;
};

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
using GenericSceneCallbackValueMap = std::tuple<std::string, SerializableContainer, std::string>;
using GenericSceneCallback = std::unordered_map<std::string, GenericSceneCallbackValueMap>;
using EventSceneCallback = GenericSceneCallback;

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
    GeomSP getNode( const UUID& _uuid );
    void addNode( GeomSP _node );
    void addNode( const UUID& _uuid );
    void removeNode( const UUID& _uuid );
    void removeNode( GeomSP _node );

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
    std::shared_ptr<Camera> DC();

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

    template <typename T>
    std::vector<std::string> getNames( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.getNames(_ref); }
    }

    template <typename T>
    [[nodiscard]] const T* get( const ResourceRef& _ref ) const {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref).get(); }
    }

    template <typename T>
    [[nodiscard]] const T* getPtr( const ResourceRef& _ref ) const {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref).get(); }
    }

    template <typename T>
    ResourceRef getHash( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.getHash(_ref); }
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
    static void addDeferred( const ResourceRef& _key, const ResourceRef& _hash, SerializableContainer&& _res, HttpResouceCB _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) resourceCallbackVData        .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) resourceCallbackRawImage     .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) resourceCallbackMaterial     .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) resourceCallbackFont         .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) resourceCallbackProfile      .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) resourceCallbackMaterialColor.emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) resourceCallbackCameraRig    .emplace_back( _key, _hash, std::move(_res), _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) resourceCallbackGeom         .emplace_back( _key, _hash, std::move(_res), _ccf );
    }
    static void addDeferredComp( SerializableContainer&& _data, HttpResouceCB _ccf = nullptr ) {
        resourceCallbackComposite.emplace_back( "", "", std::move(_data), _ccf );
    }

    template <typename R>
    ResourceRef add( const ResourceRef& _key, const R& _res, HttpResouceCB _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) return addVData        ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) return addRawImage     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) return addMaterial     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) return addFont         ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) return addProfile      ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) return addMaterialColor( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) return addCameraRig    ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) return addGeom         ( _key, _res, _ccf );
    }

    ResourceRef addVData         ( const ResourceRef& _key, const VData        & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addRawImage      ( const ResourceRef& _key, const RawImage     & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addMaterial      ( const ResourceRef& _key, const Material     & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addFont          ( const ResourceRef& _key, const Font         & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addProfile       ( const ResourceRef& _key, const Profile      & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addMaterialColor ( const ResourceRef& _key, const MaterialColor& _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addCameraRig     ( const ResourceRef& _key, const CameraRig    & _res, HttpResouceCB _ccf = nullptr );
    ResourceRef addGeom          ( const ResourceRef& _key,       GeomSP         _res, HttpResouceCB _ccf = nullptr );
    void addResources( const SerializableContainer& _data, HttpResouceCB _ccf = nullptr );

    ResourceRef addMaterialIM    ( const ResourceRef& _key, const Material     & _res );

    static void addGenericCallback( const std::string& _key, GenericSceneCallbackValueMap&& _value ) {
        SceneGraph::genericSceneCallback.emplace( _key, std::move(_value) );
    }
    static void addEventCallback( const std::string& _key, GenericSceneCallbackValueMap&& _value ) {
        SceneGraph::eventSceneCallback.emplace( _key, std::move(_value) );
    }

    template <typename T>
    T B( const std::string& _name ) {
        return T{ *this, _name };
    }

    void loadVData         ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadRawImage      ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadMaterial      ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadFont          ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadProfile       ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadMaterialColor ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadCameraRig     ( std::string _names, HttpResouceCB _ccf = nullptr );
    void loadGeom          ( std::string _names, HttpResouceCB _ccf = nullptr );

    template <typename R>
    void load( std::string _names, HttpResouceCB _ccf = nullptr ) {

        replaceAllStrings( _names, " ", "," );
        if constexpr ( std::is_same_v<R, VData          > ) loadVData        ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) loadRawImage     ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) loadMaterial     ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) loadFont         ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) loadProfile      ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) loadMaterialColor( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) loadCameraRig    ( std::move(_names), _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) loadGeom         ( std::move(_names), _ccf );
    }

    std::tuple<std::string, V3f> getGeomNameSize( const ResourceRef& _ref ) const {
        return { _ref, get<Geom>( _ref )->BBox3dPtr()->size() };
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
    GeomSP GB( Args&&... args ) {
        VDataAssembler<T> gb{std::forward<Args>(args)...};
        GeomSP elem;
        if constexpr ( !std::is_same_v<T, GT::Asset> ) {
            auto matRef     = GBMatInternal(gb.matRef, gb.matColor );

            VDataServices::prepare( *this, gb.dataTypeHolder );
            auto hashRefName = VDataServices::refName( gb.dataTypeHolder );
            auto vdataRef = VL().getHash( hashRefName );
            if ( vdataRef.empty() ) {
                vdataRef = B<VB>( hashRefName ).addIM( VDataServices::build(gb.dataTypeHolder) );
            }

            elem = std::make_shared<Geom>(gb.Name());
            elem->setTag(gb.tag);
            elem->pushData( vdataRef, get<VData>( vdataRef)->BBox3d(), matRef );

            if ( gb.elemInjFather ) gb.elemInjFather->addChildren(elem);
            elem->updateExistingTransform( gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale );
            auto ref = B<GRB>( gb.Name() ).addIM( elem );
            if ( !gb.elemInjFather ) addNode(elem);
            return elem;
        } else {
            elem = EF::clone(get<Geom>(gb.dataTypeHolder.nameId));
            elem->setTag(gb.tag);
            if ( gb.elemInjFather ) gb.elemInjFather->addChildren(elem);
            elem->updateExistingTransform( gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale );
            if ( !gb.matRef.empty() && gb.matRef != S::WHITE_PBR ) {
                auto matRef     = GBMatInternal(gb.matRef, gb.matColor );
                elem->foreach( [&matRef](GeomSP _geom) {
                    if ( !_geom->empty() ) {
                        _geom->DataRef().material = matRef;
                    }
                });
            }
            if ( !gb.elemInjFather ) addNode(elem);
        }
        return elem;
    }

    void chartMeshes( scene_t& scene );
    void chartMeshes2( scene_t& scene );

    static GenericSceneCallback           genericSceneCallback         ;
    static EventSceneCallback             eventSceneCallback           ;
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
    ResourceRef GBMatInternal( CResourceRef _matref, const C4f& _color );
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
