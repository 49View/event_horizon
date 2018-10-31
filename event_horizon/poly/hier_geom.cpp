//
//  hier_geom.cpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#include "hier_geom.hpp"
#include "core/serializebin.hpp"
#include "geom_builder.h"
#include "poly/lightmap_exchange_format.h"

int64_t HierGeom::globalHierHash = 1000000000;

HierGeom::HierGeom() {
    mHash = ++globalHierHash;
    mName = std::to_string( mHash );
    mLocalHierTransform = std::make_shared<Matrix4f>(Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ),
                                                     Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ),
                                                     Vector4f( 0.0f, 0.0f, 1.0f, 0.0f ),
                                                     Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
}

HierGeom::HierGeom( const std::string& _name ) : HierGeom() {
    mName = _name;
}

HierGeom::HierGeom( std::vector<char> _data ) : HierGeom() {
    std::shared_ptr<DeserializeBin> reader = std::make_shared<DeserializeBin>( _data  );
    gatherGeomDependencies( reader );
    deserialize( reader );
}

HierGeom::HierGeom( std::shared_ptr<GeomData> data, HierGeom *papa ) : HierGeom() {
    father = papa;
    mData = data;
}

HierGeom::HierGeom( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) : HierGeom() {
    generateLocalTransformData(pos, rot, scale);
    generateMatrixHierarchy(fatherRootTransform());
}

//HierGeom::HierGeom( const HierGeom& source ) {
//    mHash = ++globalHierHash;
//    copyTransformDataFrom( &source );
//    mName = source.Name();
//    mData = source.Geom();
//    bbox3d = source.BBox3d();
//    mSHReceiver = source.SHReceiver();
//    mHidden = std::make_shared<AnimType<float>>( source.isHidden() ? 1.0f : 0.0f );
//    father = source.Father();
//    mGHType = source.GHType();
//    CastShadows( source.CastShadows());
//    for ( const auto& c : source.Children()) {
//        addChildren( std::make_shared<HierGeom>( *c.get()));
//    }
//}

//void HierGeom::initialiseCTOR( const char *name, HierGeom *papa, const Vector3f& pos, const Vector3f& rot,
//                               const Vector3f& scale ) {
//    mHash = ++globalHierHash;
//    mName = name;
//    masterHashName = "ucarcamagnu";
//    mGHType = GHTypeGeneric;
//    father = papa;
//    bbox3d = AABB::INVALID;
//    mMinRotLimit = -TWO_PI;
//    mMaxRotLimit = TWO_PI;
//    mHidden = std::make_shared<AnimType<float>>( 0.0f );
//    MRO( MatrixRotationOrder::xyz );
//    mSHReceiver = false;
//    CastShadows( true );
//    // generateLocalTransformData relies on mMinRotLimit and mMaxRotLimit being set, do not call this function _before_ you set those variables.
//    generateLocalTransformData( pos, rot, scale );
//    mPivotTransform = Matrix4f::IDENTITY;
//    mInvPivotTransform = Matrix4f::IDENTITY;
//    mLocalHierTransform = fatherRootTransform();
//    children.clear();
//}

void HierGeom::serializeDependenciesRec( std::shared_ptr<SerializeBin> writer ) {

    if ( mData ) {
        mData->serializeDependencies( writer );
    }

    for ( auto&& c : Children()) {
        c->serializeDependenciesRec( writer );
    }
}

void HierGeom::serializeDependencies( std::shared_ptr<SerializeBin> writer ) {

    serializeDependenciesRec( writer );
    // End tag so we know there are no more dependencies
    writer->write( uint32_t(0) );
}

void HierGeom::serializeRec( std::shared_ptr<SerializeBin> writer ) {

    writer->write( mGHType );
    writer->write( mHash );
    writer->write( mName );
    writer->write( mCastShadows );
    writer->write( mSHReceiver );
    writer->write( mLocalTransform );
    writer->write( mTRS.pos->value );
    writer->write( mTRS.rot->value );
    writer->write( mTRS.scale->value );
    writer->write( bbox3d );
    int32_t hasData = mData ? 1 : 0;
    writer->write( hasData );
    if ( hasData == 1 ) mData->serialize( writer );

    int32_t numChildren = static_cast<int32_t>( Children().size());
    writer->write( numChildren );
    for ( auto&& c : Children()) {
        c->serializeRec( writer );
    }
}

void HierGeom::deserializeRec( std::shared_ptr<DeserializeBin> reader, HierGeom *_father ) {
    father = _father;
    reader->read( mGHType );
    reader->read( mHash );
    reader->read( mName );
    reader->read( mCastShadows );
    reader->read( mSHReceiver );
    reader->read( mLocalTransform );
    reader->read( mTRS.pos->value );
    reader->read( mTRS.rot->value );
    reader->read( mTRS.scale->value );
    reader->read( bbox3d );

    int32_t hasData = 0;
    reader->read( hasData );
    if ( hasData == 1 ) {
        mData = std::make_shared<GeomData>(reader);
    }

    int32_t numChildren = 0;
    reader->read( numChildren );

    for ( int32_t t = 0; t < numChildren; t++ ) {
        std::shared_ptr<HierGeom> gg = std::make_shared<HierGeom>();
        addChildren( gg );
        gg->deserializeRec( reader, this );
    }
}

std::vector<unsigned char> HierGeom::serialize() {

    auto writer = std::make_shared<SerializeBin>( SerializeVersionFormat::UInt64, entityGroup() );

    serializeDependencies( writer );
    serializeRec( writer );

    return writer->buffer();
}

bool HierGeom::deserialize( std::shared_ptr<DeserializeBin>& reader ) {
    deserializeRec( reader );
    return true;
}

HierGeom *HierGeom::root() {
    HierGeom *ret = this;

    while ( ret->Father() != nullptr ) {
        ret = ret->Father();
    }

    return ret;
}

void HierGeom::getLocatorsPosRec( std::vector<Vector3f>& _locators ) {

    if ( checkBitWiseFlag( GHType(), GHTypeLocator )) {
        Vector3f lpos = mLocalHierTransform->getPosition3();
        _locators.push_back( lpos );
    }

    for ( auto& c : children ) {
        c->getLocatorsPosRec( _locators );
    }
}

std::vector<Vector3f> HierGeom::getLocatorsPos() {
    std::vector<Vector3f> ret;

    getLocatorsPosRec( ret );

    return ret;
}

int HierGeom::totalChildrenRec( int& numC ) const {
    numC++;

    for ( auto& c : children ) {
        c->totalChildrenRec( numC );
    }

    return numC;
}

int HierGeom::totalChildren() const {
    int numC = 0;
    return totalChildrenRec( numC );
}

int HierGeom::totalChildrenWithGeomRec( int& numC ) const {
    if ( mData ) numC++;

    for ( auto& c : children ) {
        c->totalChildrenWithGeomRec( numC );
    }

    return numC;
}

int HierGeom::totalChildrenWithGeom() const {
    int numC = 0;
    return totalChildrenWithGeomRec( numC );
}

int HierGeom::totalChildrenOfTypeRec( GeomHierType _gt, int& numC ) const {
    if ( mGHType & _gt ) numC++;

    for ( auto& c : children ) {
        c->totalChildrenOfTypeRec( _gt, numC );
    }

    return numC;
}

int HierGeom::totalChildrenOfType( GeomHierType _gt ) const {
    int numC = 0;
    return totalChildrenOfTypeRec( _gt, numC );
}

void HierGeom::mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox ) {
//	if ( mData ) {
//		mData->mirrorFlip( wow, woh, bbox );
//	}
//
//	for ( auto& c : children ) {
//		c->mirrorFlip( wow, woh, bbox );
//	}
}

void HierGeom::containingAABBRec( AABB& _bbox ) const {
    if ( mData ) {
        auto cr = BBox3d();
        _bbox.merge( cr );
    }

    for ( auto& c : children ) {
        c->containingAABBRec( _bbox );
    }
}

AABB HierGeom::containingAABB() const {

    AABB ret = BBox3d();

    containingAABBRec( ret );

    return ret;
}

AABB HierGeom::calcCompleteBBox3dRec() {
    if ( mData ) {
        AABB lBBox = mData->BBox3d();
        lBBox.transform( *mLocalHierTransform );
        bbox3d = lBBox;
    }

    for ( auto& c : children ) {
        bbox3d.merge( c->calcCompleteBBox3dRec() );
    }

    return bbox3d;
}

void HierGeom::calcCompleteBBox3d() {
    bbox3d = calcCompleteBBox3dRec();
}

void HierGeom::createLocalHierMatrix( Matrix4f cmat ) {
    Matrix4f& lLHT = *mLocalHierTransform;
    lLHT = mLocalTransform * cmat;
//    if ( mData ) {
//        bbox3d = mData->BBox3d();
//        bbox3d.transform( lLHT );
//    }
}

void HierGeom::generateMatrixHierarchyRec( Matrix4f cmat ) {
    createLocalHierMatrix( cmat );

    for ( auto& c : children ) {
        c->generateMatrixHierarchyRec( *mLocalHierTransform );
    }
}

void HierGeom::generateMatrixHierarchy( Matrix4f cmat ) {
    generateMatrixHierarchyRec( cmat );
    calcCompleteBBox3d();

}

void HierGeom::numVertsRec( int& currNumVerts ) {
    currNumVerts += mData ? mData->numVerts() : 0;

    for ( auto& c : children ) {
        c->numVertsRec( currNumVerts );
    }
}

int HierGeom::numVerts() {
    int currNumVerts = 0;

    numVertsRec( currNumVerts );

    return currNumVerts;
}

void HierGeom::findRecursive( const char *name, HierGeom *& foundObj ) {
    if ( strcmp( mName.c_str(), name ) == 0 ) {
        foundObj = this;
        return;
    }

    for ( auto& c : children ) {
        c->findRecursive( name, foundObj );
        if ( foundObj != nullptr ) break;
    }
}

HierGeom *HierGeom::find( const char *name ) {
    HierGeom *foundObj = nullptr;
    findRecursive( name, foundObj );
    return foundObj;
}

void HierGeom::removeTypeRec( const GeomHierType gt ) {
    removeType( gt );

    // Traverse tree
    for ( auto& c : children ) {
        c->removeTypeRec( gt );
    }
}

void HierGeom::getGeomOfType( GeomHierType gt, std::vector<HierGeom *>& ret ) {
    if ( checkBitWiseFlag( GHType(), gt )) {
        ret.push_back( this );
    }

    // Traverse tree
    for ( auto& c : children ) {
        c->getGeomOfType( gt, ret );
    }
}

void HierGeom::getGeomWithName( const std::string& _name, std::vector<HierGeom *>& ret ) {
    if ( Name() == _name ) {
        ret.push_back( this );
    }

    // Traverse tree
    for ( auto& c : children ) {
        c->getGeomWithName( _name, ret );
    }
}

void HierGeom::removeChildrenWithHash( int64_t _hash ) {
    children.erase( remove_if( children.begin(), children.end(),
                               [_hash]( std::shared_ptr<HierGeom> const& us ) -> bool { return us->Hash() == _hash; } ),
                    children.end());

    // Traverse tree
    for ( auto& c : children ) {
        c->removeChildrenWithHash( _hash );
    }
}

void HierGeom::removeChildrenWithType( GeomHierType gt ) {
    children.erase( remove_if( children.begin(), children.end(), [gt]( std::shared_ptr<HierGeom> const& us ) -> bool {
        return checkBitWiseFlag( us->GHType(), gt );
    } ), children.end());

    // Traverse tree
    for ( auto& c : children ) {
        c->removeChildrenWithType( gt );
    }
}

void HierGeom::getGeomWithHash( int64_t _hash, HierGeom *& retG ) {
    bool stopRec = false;
    if ( Hash() == _hash ) {
        retG = this;
        stopRec = true;
    }

    // Traverse tree
    if ( !stopRec ) {
        for ( auto& c : children ) {
            c->getGeomWithHash( _hash, retG );
        }
    }
}

Matrix4f HierGeom::fatherRootTransform() const {
    if ( father == nullptr ) return Matrix4f::IDENTITY;
    return *(father->mLocalHierTransform.get());
}

void HierGeom::generateLocalTransformData( const Vector3f& pos, const Vector3f& angleAxis, const Vector3f& scale ) {
    mTRS.set( pos, angleAxis, scale );
    mLocalTransform = Matrix4f{ mTRS };
}

void HierGeom::generateLocalTransformData( const Vector3f& pos, const Quaternion& angleAxis, const Vector3f& scale ) {
    mTRS.set( pos, angleAxis, scale );
    mLocalTransform = Matrix4f{ mTRS };
}

void HierGeom::updateTransformRec( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot,
                                   const Vector3f& scale, UpdateTypeFlag whatToUpdate ) {
    if ( nodeName.compare( mName ) == 0 ) {
        generateLocalTransformData( whatToUpdate & UpdateTypeFlag::Position ? pos : mTRS.Pos(),
                                    whatToUpdate & UpdateTypeFlag::Rotation ? Quaternion{rot} : mTRS.Rot(),
                                    whatToUpdate & UpdateTypeFlag::Scale ? scale : mTRS.Scale());
    }

    for ( auto& c : children ) {
        c->updateTransformRec( nodeName, pos, rot, scale, whatToUpdate );
    }
}

void
HierGeom::updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot, const Vector3f& scale,
                           UpdateTypeFlag whatToUpdate ) {
    if ( whatToUpdate == UpdateTypeFlag::Nothing ) return;

    updateTransformRec( nodeName, pos, rot, scale, whatToUpdate );

    generateMatrixHierarchy( fatherRootTransform());
}

// In this case we choose what to update
void HierGeom::updateTransform( const std::string& nodeName, const Vector3f& val, UpdateTypeFlag whatToUpdate ) {
    updateTransform( nodeName, whatToUpdate == UpdateTypeFlag::Position ? val : mTRS.Pos(),
                     whatToUpdate == UpdateTypeFlag::Rotation ? val : mTRS.Rot().euler(),
                     whatToUpdate == UpdateTypeFlag::Scale ? val : mTRS.Scale(), whatToUpdate );
}

// Update pos and rotation
void HierGeom::updateExistingTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) {

    mTRS.set( pos, rot, scale );
    auto mm = Matrix4f{ mTRS };
    mLocalTransform = mLocalTransform * mm;

    generateMatrixHierarchy( fatherRootTransform() );
}


void HierGeom::updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot ) {
    updateTransform( nodeName, pos, rot, mTRS.Scale(),
                     static_cast<UpdateTypeFlag>( UpdateTypeFlag::Position | UpdateTypeFlag::Rotation ));
}

void HierGeom::updateTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) {
    generateLocalTransformData( pos, rot, scale );
    generateMatrixHierarchy( fatherRootTransform() );
}

void HierGeom::updateTransform( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale ) {
    generateLocalTransformData( pos, rot, scale );
    generateMatrixHierarchy( fatherRootTransform() );
}

void HierGeom::updateTransform( const Vector3f& pos, const Vector3f& rot ) {
    generateLocalTransformData( pos, rot, mTRS.Scale());
    generateMatrixHierarchy( fatherRootTransform());
}

void HierGeom::updateTransform( const Vector3f& pos ) {
    generateLocalTransformData( pos, mTRS.Rot(), mTRS.Scale());
    generateMatrixHierarchy( fatherRootTransform());
}

void HierGeom::updateTransform() {
    updateTransform( Vector3f::ZERO );
}

void HierGeom::eraseChildren( const char *name ) {
    for ( std::vector<std::shared_ptr<HierGeom>>::iterator it = children.begin(); it != children.end(); ) {
        if ( strcmp(( *( it ))->Name().c_str(), name ) == 0 ) {
            children.erase( it );  // Returns the new iterator to continue from.
            return;
        } else {
            ++it;
        }
    }
}

void HierGeom::extractHier( std::vector<std::shared_ptr<HierGeom>>& geoms, const char *name, ExtractFlags ef ) {
    if ( strcmp( mName.c_str(), name ) == 0 ) {
        geoms.push_back( clone());
        if ( ef == ExtractFlags::RemoveAfterExtract ) {
            if ( father != nullptr )
                father->eraseChildren( name );
        }
    }

    for ( auto& c : children ) {
        c->extractHier( geoms, name, ef );
    }
}

void HierGeom::commandReposition( const std::vector<std::string>& itr ) {
    std::vector<std::string> meshesToReposition;

    std::vector<std::string>::const_iterator it = itr.begin();
    while ( *( it ) != "to" ) {
        meshesToReposition.push_back( *it );
        ++it;
    }
    std::string dest = *( ++it );
    HierGeom *gd = find( dest.c_str());
    if ( !gd ) return;

    for ( auto mesh : meshesToReposition ) {
        std::vector<std::shared_ptr<HierGeom>> geoms;
        extractHier( geoms, mesh.c_str(), ExtractFlags::RemoveAfterExtract );
        for ( auto gs : geoms ) {
            gd->addChildren( gs );
        }
    }
}

void HierGeom::generateGeometryVP() {
    if ( mData->numIndices() < 3 ) return;
    ASSERT( mData->numIndices() > 2 );
    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[mData->numIndices()] );
    std::memcpy( _indices.get(), mData->Indices(), mData->numIndices() * sizeof( int32_t ));
    mSOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( mData->numVerts(), PRIMITIVE_TRIANGLES,
                                                              VFVertexAllocation::PreAllocate, mData->numIndices(),
                                                              _indices );
    for ( int32_t t = 0; t < mData->numVerts(); t++ ) {
        mSOAData->addVertex( mData->vertexAt( t ), mData->uvAt( t ), mData->uv2At( t ), mData->normalAt( t ),
                             mData->tangentAt( t ), mData->binormalAt( t ), mData->colorAt(t) );
    }

    notify( shared_from_this(), "generateGeometryVP" );
}

void HierGeom::generateSOA() {
    if ( mData ) {
        generateGeometryVP();
    }

    for ( auto& c : Children()) {
        c->generateSOA();
    }
}

void HierGeom::subscribeRec( std::shared_ptr<ObserverShared<HierGeom>> _val ) {
    if ( mData ) {
        subscribe( _val );
    }

    for ( auto& c : Children()) {
        c->subscribeRec( _val );
    }
}

void HierGeom::finalize() {
    createLocalHierMatrix(fatherRootTransform());

    for ( auto& c : Children()) {
        c->finalize();
    }
}

void HierGeom::gatherStats() {
    if ( Geom()) {
//		RL.Stats().NumTriangles() += Geom()->numIndices() / 3;
//		RL.Stats().NumIndices() += Geom()->numIndices();
//		RL.Stats().NumVerts() += Geom()->numVerts();
//		RL.Stats().NumNormals() += Geom()->numVerts();
//		++RL.Stats().NumGeoms();
    }

    for ( auto&& c : Children()) {
        c->gatherStats();
    }
}

void HierGeom::relightSH( bool includeChildren ) {
    if ( mData != nullptr && mData->numVerts() > 0 ) {
        generateGeometryVP();
    }

    if ( includeChildren )
        for ( auto&& c : Children())
            c->relightSH();
}

//void HierGeom::renderShadowMap() {
//	if ( isHidden() ) return;
//
//	if ( mVPList3d->size() > 0 ) {
//		mVPList3d->addToCommandBuffer( mLocalHierTransform * SMM.ShadowMapMVP(), MF( S::SHADOW_MAP ), 0.6f );
//	}
//
//	// Traverse tree
//	for ( auto& c : children ) {
//		c->renderShadowMap();
//	}
//}

//void HierGeom::renderScene( [[maybe_unused]] const std::string& cameraRig ) {
//	if ( isHidden() ) return;
//
//	if ( mVPList3d->size() > 0 ) {
//		if ( !CM.getCamera( cameraRig )->frustomClipping( bbox3d ) ) return;
//		mVPList3d->addToCommandBuffer( mLocalHierTransform );
//	}
//
//	// Traverse tree
//	for ( auto& c : children ) {
//		if ( c->mVPList3d->size() > 0 && c->mData->getMaterial()->Uniforms()->getFloat( UniformNames::opacity ) < 1.0f ) {
//			RL.addToTransparentList( c );
//		} else {
//			c->renderScene( cameraRig );
//		}
//	}
//}

//void HierGeom::update( float timeStamp ) {
//	//mTRS.update(timeStamp);
//
//	int whatToAnimate = UpdateTypeFlag::Nothing;
//	if ( mTRS.pos->isAnimating() ) whatToAnimate |= static_cast<int>( UpdateTypeFlag::Position );
//	if ( mTRS.angleX->isAnimating() ) whatToAnimate |= static_cast<int>( UpdateTypeFlag::Rotation );
//	if ( mTRS.angleY->isAnimating() ) whatToAnimate |= static_cast<int>( UpdateTypeFlag::Rotation );
//	if ( mTRS.angleZ->isAnimating() ) whatToAnimate |= static_cast<int>( UpdateTypeFlag::Rotation );
//	if ( mTRS.scale->isAnimating() ) whatToAnimate |= static_cast<int>( UpdateTypeFlag::Scale );
//
//	updateTransform( mName, mTRS.Pos(), Vector3f( mTRS.AngleX(), mTRS.AngleY(), mTRS.AngleZ() ), mTRS.Scale(), static_cast<UpdateTypeFlag>( whatToAnimate ) );
//
//	for ( auto& c : children ) {
//		c->update( timeStamp );
//	}
//}

void HierGeom::copyTRSDataFrom( const HierGeom *source ) {
    generateLocalTransformData( source->TRS().pos->value, source->TRS().rot->value, source->TRS().scale->value );
}

void HierGeom::copyTransformDataFrom( const HierGeom *source ) {
    copyTRSDataFrom( source );
}

std::shared_ptr<HierGeom> HierGeom::clone() const {
    return std::make_shared<HierGeom>( *this );
}

std::shared_ptr<HierGeom>
HierGeom::addChildren( std::shared_ptr<GeomData> data, const Vector3f& pos, const Vector3f& rot,
                       const Vector3f& scale, bool visible ) {
    std::shared_ptr<HierGeom> geom = std::make_shared<HierGeom>( data, this );
    geom->updateTransform( pos, rot, scale );
    children.push_back( geom );
    return geom;
}

void HierGeom::addChildren( std::shared_ptr<GeomData> data, std::shared_ptr<HierGeom> _child, const Vector3f& pos,
                            const Vector3f& rot, const Vector3f& scale, bool visible ) {
    _child = std::make_shared<HierGeom>( data, this );
    _child->updateTransform( pos, rot, scale );
    children.push_back( _child );
}

std::shared_ptr<HierGeom> HierGeom::addChildren( std::shared_ptr<HierGeom> data ) {
    data->Father( this );
    data->updateTransform();
    children.push_back( data );
    return data;
}

std::shared_ptr<HierGeom> HierGeom::addChildren( const std::string& name ) {
    std::shared_ptr<HierGeom> data = std::make_shared<HierGeom>();
    data->Father( this );
    data->Name( name );
    data->updateTransform();
    children.push_back( data );
    return data;
}

std::shared_ptr<HierGeom> HierGeom::addChildren( const Vector3f& pos,
                                                 const Vector3f& rot,
                                                 const Vector3f& scale) {
    std::shared_ptr<HierGeom> data = std::make_shared<HierGeom>(pos, rot, scale);
    data->Father( this );
    data->updateTransform( pos, rot, scale );
    children.push_back( data );
    return data;
}

bool HierGeom::isDescendantOf( const int64_t& ancestorHash ) {
    if ( mHash == ancestorHash ) return true;

    auto f = father;
    while ( f ) {
        if ( f->mHash == ancestorHash ) return true;
        f = f->father;
    }

    return false;
}

Vector3f HierGeom::position() const {
    return mLocalHierTransform->getPosition3();
}


void HierGeom::pruneRec() {

    using vi = std::vector<std::shared_ptr<HierGeom>>::iterator;

    for ( vi it = children.begin(); it != children.end(); ) {
        if ( (*it)->Children().empty() && !((*it)->Geom()) ) {
            it = children.erase( it );
        } else {
            (*it)->pruneRec();
            ++it;
        }
    }
}

void HierGeom::prune() {
    pruneRec();
}