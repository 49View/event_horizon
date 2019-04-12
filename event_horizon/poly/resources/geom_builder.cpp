//
// Created by Dado on 29/10/2017.
//

#include "geom_builder.h"
#include <core/names.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/resources/resource_builder.hpp>
#include <core/geom.hpp>
#include <poly/poly.hpp>

GeomBuilder::GeomBuilder( SceneGraph& _sg ) : sg(_sg) {

}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt ) : builderType(gbt), sg(_sg) {
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt, const std::string& _name ) : builderType(gbt),sg(_sg) {
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, std::initializer_list<Vector2f>&& arguments_list, float _zPull ) : sg(_sg) {
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back(v);
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<Vector3f>& arguments_list, float _zPull ) : sg(_sg) {
    outlineVerts.emplace_back( arguments_list, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<Vector2f>& arguments_list, float _zPull ) : sg(_sg) {
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back(v) ;
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, ShapeType _st, const Vector3f& _size ) : sg(_sg){
    shapeType = _st;
    scale = _size;
    builderType = GeomBuilderType::shape;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt, const std::string& _resourceName,
                          const std::vector<Vector2f>& _outline,
                          const float _z, const Vector3f& _suggestedAxis ) : sg(_sg) {
    builderType = gbt;
    mDepResourceName = _resourceName;
    for (auto &v: _outline) profilePath.emplace_back( Vector3f{v, _z} );
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt, const std::string& _resourceName,
                          const std::vector<Vector3f>& _outline,
                          const Vector3f& _suggestedAxis ) : sg(_sg) {
    builderType = gbt;
    mDepResourceName = _resourceName;
    mFollowerSuggestedAxis = _suggestedAxis;
    for (auto &v: _outline) profilePath.emplace_back( v );
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt, const std::string& _resourceName,
                          const Rect2f& _r, const Vector3f& _suggestedAxis ) : sg(_sg) {
    builderType = gbt;
    mDepResourceName = _resourceName;
    mFollowerSuggestedAxis = _suggestedAxis;
    for ( auto &v: _r.points3dcw() ) profilePath.emplace_back( v );
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, std::initializer_list<Vector3f>&& arguments_list, float _zPull )
                        : sg(_sg) {
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back( v );
    outlineVerts.emplace_back( lverts, _zPull );
    builderType = GeomBuilderType::outline;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const GeomBuilderType gbt, const std::initializer_list<std::string>& _tags ) : builderType(gbt), sg(_sg) {
    Name(concatenate( "_", _tags ));
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const Rect2f& _rect, float _z ) : sg(_sg) {
    builderType = GeomBuilderType::poly;
    sourcePolysVList = XZY::C(_rect.points3d(_z));
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<Vector3f>& _vlist ) : sg(_sg) {
    builderType = GeomBuilderType::poly;
    sourcePolysVList = _vlist;
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<Triangle2d>& _tris, float _z ) : sg(_sg) {
    builderType = GeomBuilderType::poly;
    for ( const auto& [v1,v2,v3] : _tris ) {
        sourcePolysTris.emplace_back(Triangle3d( {v1, _z}, {v2, _z}, {v3, _z} ) );
    }
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<PolyLine2d>& _plines, float _z ) : sg(_sg) {
    builderType = GeomBuilderType::poly;

    for ( const auto& p : _plines ) {
        addPoly( p, _z );
    }
}

GeomBuilder::GeomBuilder( SceneGraph& _sg, const std::vector<PolyLine>& _plist ) : sg(_sg) {
    polyLines = _plist;
}

void GeomBuilder::createFromProcedural( std::shared_ptr<GeomDataBuilder> gb ) {
    gb->setupRefName();
    auto rna = sg.VL().getHash( gb->refName() );
    elem->VDataRef( rna.empty() ? sg.B<VB>( gb->refName() ).addIM( gb->build() ) : rna );
}

void GeomBuilder::createFromProcedural( std::shared_ptr<GeomDataBuilderList> gb ) {
    for ( const auto& c : gb->build() ) {
        sg.B<VB>( "urca" ).addIM( c );
//    ### REF re-implement this
//        elem->addChildren( c );
    }
}

void GeomBuilder::createFromAsset( GeomAssetSP asset ) {
    elem->addChildren( asset );
}

void GeomBuilder::build() {
    if ( Name().empty() ) {
        Name( UUIDGen::make() );
    }

    elemCreate();

    if ( matRef.empty() ) matRef = S::WHITE_PBR;
    elem->MaterialRef( sg.ML().getHash(matRef) );

    switch ( builderType ) {
        case GeomBuilderType::shape:
            createFromProcedural( std::make_shared<GeomDataShapeBuilder>( shapeType ) );
            break;
        case GeomBuilderType::outline:
            createFromProcedural( std::make_shared<GeomDataOutlineBuilder>( outlineVerts ) );
            break;
        case GeomBuilderType::poly:
            preparePolyLines();
            createFromProcedural( std::make_shared<GeomDataPolyBuilder>( polyLines ) );
            break;
        case GeomBuilderType::mesh:
            createFromProcedural( std::make_shared<GeomDataQuadMeshBuilder>( quads ) );
            break;
        case GeomBuilderType::follower: {
            createFromProcedural( std::make_shared<GeomDataFollowerBuilder>( sg.PL().get( mDepResourceName ),
                                                                             profilePath,
                                                                             fflags,
                                                                             fraise,
                                                                             flipVector,
                                                                             mGaps,
                                                                             mFollowerSuggestedAxis ) );
        }
        break;
        case GeomBuilderType::svg:
            createFromProcedural( std::make_shared<GeomDataSVGBuilder>( asciiText, sg.PL().get( mDepResourceName ) ) );
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

    sg.B<GRB>(Name()).addIM( elem );
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
    if ( !elem) elem = std::make_shared<Geom>(Name());
}

GeomBuilder& GeomBuilder::inj( GeomAssetSP _hier ) {
    elemInjFather = _hier;
    return *this;
}

GeomAssetSP GeomBuilder::buildr() {
    build();
    return elem;
}

GeomBuilder& GeomBuilder::pb( const float _a, const float _b ) {
//    ProfileBuilder{ sg.PL(), _a, _b }.build();
    return *this;
}

//bool GeomFileAssetBuilder::makeImpl( uint8_p&& _data, const DependencyStatus _status ) {
//
//    if ( _status == DependencyStatus::LoadedSuccessfully ) {
//        auto asset = EF::create<GeomAsset>( std::move(_data) );
//        asset->updateTransform();
//        mm.add( asset );
//        return true;
//    }
//    return false;
//}

GeomBuilderComposer::GeomBuilderComposer() {
    elem = std::make_shared<Geom>();
}

void GeomBuilderComposer::add( GeomBuilder _gb ) {
    builders.emplace_back( std::move(_gb.inj(elem)) );
}

void GeomBuilderComposer::build() {
    for ( auto& b : builders ) {
        b.build();
    }
}

GeomAssetSP GeomBuilderComposer::Elem() {
    return elem;
}
