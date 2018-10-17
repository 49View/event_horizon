//
// Created by Dado on 29/10/2017.
//

#include "geom_builder.h"

std::map<std::string, std::shared_ptr<HierGeom>> geomCache;

GeomBuilder::GeomBuilder( std::initializer_list<Vector3f>&& arguments_list, float _zPull ) {
    std::vector<Vector3f> lverts;
    for (auto &v: arguments_list) lverts.emplace_back(std::move(v));
    outlineVerts.push_back( { lverts, _zPull } );
    builderType = GeomBuilderType::outline;
}

void GeomBuilder::deserializeDependencies( DependencyMaker& _md ) {

    SceneGraph& sg = static_cast<SceneGraph&>(_md);

    auto reader = std::make_shared<DeserializeBin>( sg.AL().get( Name()));
    auto deps = gatherGeomDependencies( reader );

    for ( const auto& d : deps.textureDeps ) {
        addDependency<ImageBuilder>( d, sg.TL());
    }
    for ( const auto& d : deps.materialDeps ) {
        addDependency<MaterialBuilder>( d, sg.ML());
    }
}

void GeomBuilder::createDependencyList( DependencyMaker& _md ) {

    SceneGraph& sg = static_cast<SceneGraph&>(_md);

    if ( builderType == GeomBuilderType::file ) {
        addDependency<GeomFileAssetBuilder>( Name(), sg.AL());
    } else {
        addDependency<MaterialBuilder>( materialName, materialType, shaderName, sg.ML());
        if ( builderType == GeomBuilderType::follower ) {
            addDependency<ProfileBuilder>( mProfileSchema.name, sg.PL());
        }
    }

    addDependencies( std::make_shared<GeomBuilder>( *this ), _md );
}

void GeomBuilder::createFromProcedural( std::shared_ptr<GeomDataBuilder> gb, SceneGraph& sg ) {
    std::shared_ptr<GeomData> geom =
            gb->m( std::dynamic_pointer_cast<PBRMaterial>(sg.ML().get( materialName, shaderName ))).build();
    if ( elem ) {
        elem->Geom( geom );
    } else {
        elem = std::make_shared<HierGeom>( geom );
    }
    elem->GHType(gt);
}

void GeomBuilder::createFromAsset( std::shared_ptr<HierGeom> asset ) {
    if ( elem ) {
        elem->addChildren( asset );
    } else {
        elem = asset;
    }
}

void GeomBuilder::assemble( DependencyMaker& _md ) {

    SceneGraph& sg = static_cast<SceneGraph&>(_md);
    std::unique_ptr<GeomDataBuilder> gb;

    switch ( builderType ) {
        case GeomBuilderType::file:
            if ( auto ret = sg.AL().findHier( Name()); ret != nullptr ) {
                createFromAsset( ret->clone());
            } else {
                builderType = GeomBuilderType::asset;
                deserializeDependencies( _md );
                addDependencies( std::make_shared<GeomBuilder>( *this ), _md );
                return;
            }
        case GeomBuilderType::asset: {
            auto asset = std::make_shared<HierGeom>( sg.AL().get( Name()));
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
            createFromProcedural( std::make_shared<GeomDataPolyBuilder>( polyLines ), sg );
            break;
        case GeomBuilderType::mesh:
            createFromProcedural( std::make_shared<GeomDataQuadMeshBuilder>( quads ), sg );
            break;
        case GeomBuilderType::follower: {
            auto fbg = std::make_shared<GeomDataFollowerBuilder>( sg.PL().get( mProfileSchema.name ), profilePath );
            fbg->ff( mProfileSchema.flags );
            fbg->raise( mProfileSchema.raise);
            fbg->flip( mProfileSchema.flipVector );
            fbg->gaps(mGaps);
            createFromProcedural( fbg, sg );
            elem->Name( mProfileSchema.name );
            elem->GHType(gt);
        }
            break;
        case GeomBuilderType::unknown:
            LOGE( "Unknown builder type" );
            return;
        default:
            break;
    }

    elem->updateExistingTransform( pos, axis, scale );

    sg.add( elem );
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
    polyLines.push_back( _polyLine );
    return *this;
}

GeomBuilder& GeomBuilder::addPoly( const PolyLine2d& _polyLine2d, const float heightOffset ) {
    std::vector<Vector3f> vlist;
    for ( auto& v: _polyLine2d.verts ) vlist.emplace_back( XZY::C( v, heightOffset ) );
    polyLines.push_back( { vlist, _polyLine2d.normal, _polyLine2d.reverseFlag } );
    return *this;
}

GeomBuilder& GeomBuilder::addOutline( const std::vector<Vector3f>& _polyLine, const float _raise ) {
    outlineVerts.push_back( { _polyLine, _raise } );
    return *this;
}
