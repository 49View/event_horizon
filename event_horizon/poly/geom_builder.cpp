//
// Created by Dado on 29/10/2017.
//

#include "geom_builder.h"
#include <core/node.hpp>
#include <poly/poly.hpp>

GeomBuilder::GeomBuilder( GeomAssetSP _h, const std::vector<std::shared_ptr<MaterialBuilder>>& _mbs ) : MaterialBuildable(S::SH, S::WHITE){
    builderType = GeomBuilderType::import;
    elem = _h;
    matBuilders = _mbs;
    Name( _h->Name() );
}

GeomBuilder::GeomBuilder( std::initializer_list<Vector2f>&& arguments_list, float _zPull ) : MaterialBuildable(S::SH, S::WHITE){
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back(v);
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( const std::vector<Vector3f>& arguments_list, float _zPull ) : MaterialBuildable(S::SH, S::WHITE){
    outlineVerts.emplace_back( arguments_list, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( const std::vector<Vector2f>& arguments_list, float _zPull ) : MaterialBuildable(S::SH, S::WHITE){
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back(v) ;
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( ShapeType _st, const Vector3f& _size ) : MaterialBuildable(S::SH, S::WHITE){
    shapeType = _st;
    scale = _size;
    builderType = GeomBuilderType::shape;
}

GeomBuilder::GeomBuilder( const ProfileBuilder& _ps, const std::vector<Vector2f>& _outline, const float _z,
                          const Vector3f& _suggestedAxis ) : MaterialBuildable(S::SH, S::WHITE){
    mProfileBuilder = _ps;
    for (auto &v: _outline) profilePath.emplace_back( Vector3f{v, _z} );
    builderType = GeomBuilderType::follower;
}

GeomBuilder::GeomBuilder( const ProfileBuilder& _ps, const std::vector<Vector3f>& _outline,
                          const Vector3f& _suggestedAxis ) : MaterialBuildable(S::SH, S::WHITE) {
    mProfileBuilder = _ps;
    mFollowerSuggestedAxis = _suggestedAxis;
    for (auto &v: _outline) profilePath.emplace_back( v );
    builderType = GeomBuilderType::follower;
}

GeomBuilder::GeomBuilder( const ProfileBuilder& _ps, const Rect2f& _r, const Vector3f& _suggestedAxis ) : MaterialBuildable(S::SH, S::WHITE) {
    mProfileBuilder = _ps;
    mFollowerSuggestedAxis = _suggestedAxis;
    for ( auto &v: _r.points3dcw() ) profilePath.emplace_back( v );
    builderType = GeomBuilderType::follower;
}

GeomBuilder::GeomBuilder( std::initializer_list<Vector3f>&& arguments_list, float _zPull ) : MaterialBuildable(S::SH, S::WHITE) {
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back( v );
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( const GeomBuilderType gbt, const std::initializer_list<std::string>& _tags ) : MaterialBuildable(S::SH, S::WHITE), builderType(gbt) {
    Name(concatenate( "_", _tags ));
}

GeomBuilder::GeomBuilder( const Rect2f& _rect, float _z ) : MaterialBuildable(S::SH, S::WHITE) {
    builderType = GeomBuilderType::poly;
    sourcePolysVList = XZY::C(_rect.points3d(_z));
}

GeomBuilder::GeomBuilder( const std::vector<Vector3f>& _vlist ) : MaterialBuildable(S::SH, S::WHITE) {
    builderType = GeomBuilderType::poly;
    sourcePolysVList = _vlist;
}

GeomBuilder::GeomBuilder( const std::vector<Triangle2d>& _tris, float _z ) : MaterialBuildable(S::SH, S::WHITE){
    builderType = GeomBuilderType::poly;
    for ( const auto& [v1,v2,v3] : _tris ) {
        sourcePolysTris.emplace_back(Triangle3d( {v1, _z}, {v2, _z}, {v3, _z} ) );
    }
}

GeomBuilder::GeomBuilder( const std::vector<PolyLine2d>& _plines, float _z ) : MaterialBuildable(S::SH, S::WHITE){
    builderType = GeomBuilderType::poly;

    for ( const auto& p : _plines ) {
        addPoly( p, _z );
    }
}

GeomBuilder::GeomBuilder( const std::vector<PolyLine>& _plist ) : MaterialBuildable(S::SH, S::WHITE){
    polyLines = _plist;
}

void GeomBuilder::deserializeDependencies( DependencyMaker& _md ) {

    auto& sg = static_cast<SceneGraph&>(_md);

    auto reader = std::make_shared<DeserializeBin>(sg.AL().get( Name()), GeomData::Version());
    auto deps = GeomData::gatherDependencies( reader );

    for ( const auto& d : deps.textureDeps ) {
        addDependency<ImageBuilder>( d, sg.TL());
    }
    for ( const auto& d : deps.materialDeps ) {
        addDependency<MaterialBuilder>( d, sg.ML() );
    }
}

void GeomBuilder::createDependencyList( DependencyMaker& _md ) {

    auto& sg = static_cast<SceneGraph&>(_md);

    if ( builderType != GeomBuilderType::import ) {
        if ( builderType == GeomBuilderType::file ) {
            addDependency<GeomFileAssetBuilder>( Name(), sg.AL());
        } else {
            addDependency<MaterialBuilder>( material->Name(), sg.ML());
            if ( builderType == GeomBuilderType::follower || builderType == GeomBuilderType::svg ) {
                addDependency<ProfileBuilder>( mProfileBuilder, sg.PL());
            }
        }
    }

    addDependencies( std::make_shared<GeomBuilder>( *this ), _md );
}

void GeomBuilder::createFromProcedural( std::shared_ptr<GeomDataBuilder> gb, SceneGraph& sg ) {
    auto mat = std::dynamic_pointer_cast<Material>(sg.ML().get( material->Name() ));
    gb->m(mat);
    elem->Data( gb->build() );
    elem->GHType(gt);
}

void GeomBuilder::createFromProcedural( std::shared_ptr<GeomDataBuilderList> gb, SceneGraph& sg ) {
    auto mat = std::dynamic_pointer_cast<Material>(sg.ML().get( material->Name() ));
    gb->m(mat);
    for ( const auto& c : gb->build() ) {
        elem->addChildren( c );
    }
    elem->GHType(gt);
}

void GeomBuilder::createFromAsset( GeomAssetSP asset ) {
    elem->addChildren( asset );
}

void GeomBuilder::assemble( DependencyMaker& _md ) {

    auto& sg = dynamic_cast<SceneGraph&>(_md);
    std::unique_ptr<GeomDataBuilder> gb;

    switch ( builderType ) {
        case GeomBuilderType::import:
            sg.add( matBuilders );
        break;
        case GeomBuilderType::file:
            if ( auto ret = sg.AL().findHier( Name()); ret != nullptr ) {
                createFromAsset( ret->clone() );
            } else {
                builderType = GeomBuilderType::asset;
                deserializeDependencies( _md );
                addDependencies( std::make_shared<GeomBuilder>( *this ), _md );
                return;
            }
        case GeomBuilderType::asset: {
            auto asset = std::make_shared<GeomAsset>( sg.AL().get(Name()) );
            sg.AL().add( Name(), asset );
            createFromAsset( asset );
        }
        break;
        case GeomBuilderType::shape:
            createFromProcedural( std::make_shared<GeomDataShapeBuilder>( shapeType, pos, axis, scale ), sg );
            break;
        case GeomBuilderType::outline:
            createFromProcedural( std::make_shared<GeomDataOutlineBuilder>( outlineVerts ), sg );
            break;
        case GeomBuilderType::poly:
            preparePolyLines();
            createFromProcedural( std::make_shared<GeomDataPolyBuilder>( polyLines ), sg );
            break;
        case GeomBuilderType::mesh:
            createFromProcedural( std::make_shared<GeomDataQuadMeshBuilder>( quads ), sg );
            break;
        case GeomBuilderType::follower: {
            createFromProcedural( std::make_shared<GeomDataFollowerBuilder>( sg.PL().get( mProfileBuilder.Name() ),
                                                                             profilePath,
                                                                             fflags,
                                                                             fraise,
                                                                             flipVector,
                                                                             mGaps,
                                                                             mFollowerSuggestedAxis ), sg );
        }
        break;
        case GeomBuilderType::svg:
            createFromProcedural( std::make_shared<GeomDataSVGBuilder>( asciiText,
                                                                        sg.PL().get( mProfileBuilder.Name() ) ),
                                                                        sg );
            break;
        case GeomBuilderType::unknown:
            LOGE( "Unknown builder type" );
            return;
        default:
            break;
    }

    if ( elemInjFather ) {
        elemInjFather->addChildren(elem);
    }

    elem->updateExistingTransform( pos, axis, scale );

    if ( bAddToSceneGraph ) {
        sg.add( std::static_pointer_cast<GeomAsset >(elem) );
    }
}

GeomBuilder& GeomBuilder::addQuad( const QuadVector3fNormal& quad,
                                   [[maybe_unused]] bool reverseIfTriangulated ) {
    quads.push_back( quad );
    return *this;
}

bool GeomBuilder::validate() const {
    return true;
}

GeomBuilder& GeomBuilder::addPoly( const PolyLine& _polyLine ) {
    builderType = GeomBuilderType::poly;
    polyLines.push_back( _polyLine );
    return *this;
}

GeomBuilder& GeomBuilder::addPoly( const PolyLine2d& _polyLine2d, const float heightOffset ) {
    builderType = GeomBuilderType::poly;
    std::vector<Vector3f> vlist;
    for ( auto& v: _polyLine2d.verts ) vlist.emplace_back( XZY::C( v, heightOffset ) );
    // Using XZY::C transformation requires a _normal invert_ as handness changes with that transportation.
    polyLines.emplace_back( vlist, -_polyLine2d.normal, _polyLine2d.reverseFlag );
    return *this;
}

GeomBuilder& GeomBuilder::addOutline( const std::vector<Vector3f>& _polyLine, const float _raise ) {
    outlineVerts.emplace_back( _polyLine, _raise );
    return *this;
}

std::string GeomBuilder::generateThumbnail() const {
    if ( thumb ) {
        return { thumb->begin(), thumb->end() };
    }
    return std::string();
}

std::string GeomBuilder::generateRawData() const {
    return elem ? elem->serialize() : "";
}

std::string GeomBuilder::toMetaData() const {
    MegaWriter writer;

    writer.StartObject();
    writer.serialize( CoreMetaData{Name(), EntityGroup::Geom, GeomData::Version(),
                                   generateThumbnail(), generateRawData(), generateTags()} );
    writer.serialize( "BBox3d", elem->BBox3d() );
    writer.EndObject();

    return writer.getString();
}

ScreenShotContainerPtr& GeomBuilder::Thumb() {
    if ( !thumb ) {
        thumb = std::make_shared<ScreenShotContainer>();
    }
    return thumb;
}

void GeomBuilder::publish() const {
    // Publish all materials first

    for ( const auto& mb : matBuilders ) {
        mb->publish();
    }

    Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
}

void internalCheckPolyNormal( Vector3f& ln, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ReverseFlag rf ) {
    if ( ln == Vector3f::ZERO ) {
        ln = normalize( crossProduct( v1,v2,v3 ));
        if ( rf == ReverseFlag::True ) ln*=-1.0f;
    }
}

void GeomBuilder::preparePolyLines() {
    if ( polyLines.empty() ) {
        Vector3f ln = forcingNormalPoly;
        if ( !sourcePolysTris.empty() ) {
            auto [v1,v2,v3] = sourcePolysTris[0];
            internalCheckPolyNormal( ln, v1, v2, v3, rfPoly );
            for ( const auto& tri : sourcePolysTris ) {
                polyLines.emplace_back(PolyLine{ tri, ln, rfPoly});
            }
        }
        if ( !sourcePolysVList.empty() ) {
            internalCheckPolyNormal( ln, sourcePolysVList.at(0), sourcePolysVList.at(1), sourcePolysVList.at(2), rfPoly );
            polyLines.emplace_back( PolyLine{ sourcePolysVList, ln, rfPoly } );
        }
    }
}

void GeomBuilder::elemCreate() {
    if ( !elem) elem = std::make_shared<GeomAsset>();
}

GeomBuilder& GeomBuilder::inj( GeomAssetSP _hier ) {
    elemInjFather = _hier;
    return *this;
}

GeomAssetSP GeomBuilder::buildr( DependencyMaker& _md ) {
    build( _md );
    return elem;
}

bool GeomFileAssetBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    AssetManager& sg = static_cast<AssetManager&>(_md);

    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        sg.add( *this, zlibUtil::inflateFromMemory( std::move(_data) ) );
    } else {
        return false;
    }

    return true;
}
