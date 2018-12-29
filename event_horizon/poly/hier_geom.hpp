//
//  hier_geom.hpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#pragma once

#include "core/math/quaternion.h"
#include "core/observable.h"
#include "core/math/matrix_anim.h"
#include "core/math/rect2f.h"
#include "core/math/aabb.h"
#include "core/uuid.hpp"
#include "core/observable.h"
#include "core/serialization.hpp"
#include "core/serializebin.hpp"

enum class ExtractFlags {
    LeaveAsItAfterExtract,
    RemoveAfterExtract,
};

enum UpdateTypeFlag {
    Nothing = 0,
    Position = 1,
    Rotation = 1 << 1,
    Scale = 1 << 2
};

inline constexpr static uint64_t NodeVersion( const uint64_t dataVersion ) { return (2040 * 1000000) + dataVersion; }

template <typename D>
class Hier : public ObservableShared<Hier<D>>, public std::enable_shared_from_this<Hier<D>>{
public:
    Hier() {
        mHash = UUIDGen::make();
        mName = "Default";
        mLocalHierTransform = std::make_shared<Matrix4f>(Matrix4f::IDENTITY);
    }

    explicit Hier( const std::string& _name ) : Hier() {
        mName = _name;
    }
    explicit Hier( std::shared_ptr<D> data, Hier *papa = nullptr ) : Hier() {
        father = papa;
        mData = data;
    }
    Hier( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) : Hier() {
        generateLocalTransformData(pos, rot, scale);
        generateMatrixHierarchy(fatherRootTransform());
    }
    explicit Hier( std::vector<char> _data ) : Hier() {
        std::shared_ptr<DeserializeBin> reader = std::make_shared<DeserializeBin>( _data, D::Version() );
        D::gatherDependencies( reader );
        deserialize( reader );
    }

    UUID Hash() const { return mHash; }
    std::string Name() const { return mName; }
    void Name( std::string val ) { mName = val; }
    GeomHierType GHType() const { return mGHType; }
    void GHType( GeomHierType val ) { mGHType |= val; }
    bool hasType( GeomHierType val ) const { return ( mGHType & val ) > 0; }
    void removeType( GeomHierType val ) { mGHType = mGHType & ~val; }

    void sendNotifyData( const std::string& _event ) {
        if ( mData ) {
            this->notify( this->shared_from_this(), _event );
        }

        for ( auto& c : Children()) {
            c->sendNotifyData( _event );
        }
    }
    void subscribeData( std::shared_ptr<ObserverShared<Hier<D>>> _val ) {
        if ( mData ) {
            this->subscribe( _val );
        }

        for ( auto& c : Children()) {
            c->subscribeData( _val );
        }
    }

    std::vector<Vector3f> getLocatorsPos() {
        std::vector<Vector3f> ret;
        getLocatorsPosRec( ret );
        return ret;
    }

    int totalChildren() const {
        int numC = 0;
        return totalChildrenRec( numC );
    }

    int totalChildrenWithGeom() const {
        int numC = 0;
        return totalChildrenWithGeomRec( numC );
    }

    int totalChildrenOfType( GeomHierType _gt ) const {
        int numC = 0;
        return totalChildrenOfTypeRec( _gt, numC );
    }

    void copyTRSDataFrom( const Hier *source ) {
        generateLocalTransformData( source->TRS().pos->value, source->TRS().rot->value, source->TRS().scale->value );
    }
    void copyTransformDataFrom( const Hier *source ) {
        copyTRSDataFrom( source );
    }
//    void commandReposition( const std::vector<std::string>& itr );

    std::shared_ptr<Hier<D>> clone() {
        return std::make_shared<Hier<D>>( *this );
    }

    std::shared_ptr<Hier> addChildren( std::shared_ptr<D> data, const Vector3f& pos = Vector3f::ZERO,
                                           const Vector3f& rot = Vector3f::ZERO,
                                           const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        std::shared_ptr<Hier> geom = std::make_shared<Hier<D>>( data, this );
        geom->updateTransform( pos, rot, scale );
        children.push_back( geom );
        return geom;
    }

    void addChildren( std::shared_ptr<D> data, std::shared_ptr<Hier<D>> _child, const Vector3f& pos = Vector3f::ZERO,
            const Vector3f& rot = Vector3f::ZERO, const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        _child = std::make_shared<Hier<D>>( data, this );
        _child->updateTransform( pos, rot, scale );
        children.push_back( _child );
    }

    std::shared_ptr<Hier<D>> addChildren( std::shared_ptr<Hier<D>> data ) {
        data->Father( this );
        data->updateTransform();
        children.push_back( data );
        return data;
    }
    std::shared_ptr<Hier<D>> addChildren( const std::string& name ) {
        std::shared_ptr<Hier<D>> data = std::make_shared<Hier<D>>();
        data->Father( this );
        data->Name( name );
        data->updateTransform();
        children.push_back( data );
        return data;
    }
    std::shared_ptr<Hier<D>> addChildren( const Vector3f& pos = Vector3f::ZERO,
                                           const Vector3f& rot = Vector3f::ZERO,
                                           const Vector3f& scale = Vector3f::ONE ) {
        std::shared_ptr<Hier<D>> data = std::make_shared<Hier<D>>(pos, rot, scale);
        data->Father( this );
        data->updateTransform( pos, rot, scale );
        children.push_back( data );
        return data;
    }


    void generateMatrixHierarchy( Matrix4f cmat = Matrix4f::IDENTITY ) {
        generateMatrixHierarchyRec( cmat );
        calcCompleteBBox3d();
    }
    void updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot, const Vector3f& scale,
                          UpdateTypeFlag whatToUpdate ) {
        if ( whatToUpdate == UpdateTypeFlag::Nothing ) return;
        updateTransformRec( nodeName, pos, rot, scale, whatToUpdate );
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot ) {
        updateTransform( nodeName, pos, rot, mTRS.Scale(),
                         static_cast<UpdateTypeFlag>( UpdateTypeFlag::Position | UpdateTypeFlag::Rotation ));
    }
    void updateTransform( const std::string& nodeName, const Vector3f& val, UpdateTypeFlag whatToUpdate ) {
        updateTransform( nodeName, whatToUpdate == UpdateTypeFlag::Position ? val : mTRS.Pos(),
                         whatToUpdate == UpdateTypeFlag::Rotation ? val : mTRS.Rot().euler(),
                         whatToUpdate == UpdateTypeFlag::Scale ? val : mTRS.Scale(), whatToUpdate );
    }

    void updateTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) {
        generateLocalTransformData( pos, rot, scale );
        generateMatrixHierarchy( fatherRootTransform() );
    }
    void updateTransform( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale ) {
        generateLocalTransformData( pos, rot, scale );
        generateMatrixHierarchy( fatherRootTransform() );
    }
    void updateTransform( const Vector3f& pos, const Vector3f& rot ) {
        generateLocalTransformData( pos, rot, mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }
    void updateTransform( const Vector3f& pos ) {
        generateLocalTransformData( pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }
    void updateTransform() {
        updateTransform( Vector3f::ZERO );
    }

    void generateLocalTransformData( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale = Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
    }
    void generateLocalTransformData( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale=Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
    }

    Hier* find( const char *name ) {
        Hier *foundObj = nullptr;
        findRecursive( name, foundObj );
        return foundObj;
    }
    void eraseChildren( const char *name ) {
        for ( auto it = children.begin(); it != children.end(); ) {
            if ( strcmp(( *( it ))->Name().c_str(), name ) == 0 ) {
                children.erase( it );  // Returns the new iterator to continue from.
                return;
            } else {
                ++it;
            }
        }
    }

    Matrix4f RootTransform() const { return mLocalTransform; }
    void RootTransform( const Matrix4f& val ) { mLocalTransform = val; }
    std::vector<std::shared_ptr<Hier>> Children() const { return children; }
    std::vector<std::shared_ptr<Hier>>& Children() { return children; }

    std::shared_ptr<D> Geom() { return mData; }
    const std::shared_ptr<D> Geom() const { return mData; }
    void Geom( std::shared_ptr<D> val ) { mData = val; }

    void removeTypeRec( GeomHierType gt ) {
        removeType( gt );

        // Traverse tree
        for ( auto& c : children ) {
            c->removeTypeRec( gt );
        }
    }
    void getGeomOfType( GeomHierType gt, std::vector<Hier *>& ret ) {
        if ( checkBitWiseFlag( GHType(), gt )) {
            ret.push_back( this );
        }

        // Traverse tree
        for ( auto& c : children ) {
            c->getGeomOfType( gt, ret );
        }
    }
    void getGeomWithName( const std::string& _name, std::vector<Hier *>& ret ) {
        if ( Name() == _name ) {
            ret.push_back( this );
        }

        // Traverse tree
        for ( auto& c : children ) {
            c->getGeomWithName( _name, ret );
        }
    }
    void getGeomWithHash( const UUID& _hash, Hier *& retG ) {
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

    void Father( Hier *val ) { father = val; }
    Hier *Father() { return father; }
    Hier *Father() const { return father; }
    std::shared_ptr<Matrix4f> getLocalHierTransform() { return mLocalHierTransform; }
    void updateExistingTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) {
        mTRS.set( pos, rot, scale );
        auto mm = Matrix4f{ mTRS };
        mLocalTransform = mLocalTransform * mm;

        generateMatrixHierarchy( fatherRootTransform() );
    }
    Matrix4f fatherRootTransform() const {
        if ( father == nullptr ) return Matrix4f::IDENTITY;
        return *(father->mLocalHierTransform.get());
    }
    Matrix4f LocalTransform() const { return mLocalTransform; }
    void LocalTransform( const Matrix4f& m ) { mLocalTransform = m; }

    Vector3f position() const {
        return mLocalHierTransform->getPosition3();
    }

    void removeChildrenWithHash( const UUID& _hash ) {
        children.erase( remove_if( children.begin(), children.end(),
                                   [_hash]( std::shared_ptr<Hier> const& us ) -> bool { return us->Hash() == _hash; } ),
                        children.end());

        // Traverse tree
        for ( auto& c : children ) {
            c->removeChildrenWithHash( _hash );
        }
    }
    void removeChildrenWithType( GeomHierType gt ) {
        children.erase( remove_if( children.begin(), children.end(), [gt]( std::shared_ptr<Hier> const& us ) -> bool {
            return checkBitWiseFlag( us->GHType(), gt );
        } ), children.end());

        // Traverse tree
        for ( auto& c : children ) {
            c->removeChildrenWithType( gt );
        }
    }

    void prune() {
        pruneRec();
    }
    void finalize() {
        createLocalHierMatrix(fatherRootTransform());
        for ( auto& c : Children()) {
            c->finalize();
        }
    }

    std::vector<unsigned char> serialize() {
        auto writer = std::make_shared<SerializeBin>( D::Version() );

        serializeDependencies( writer );
        serializeRec( writer );

        return writer->buffer();
    }

    bool deserialize( std::shared_ptr<DeserializeBin>& reader ) {
        deserializeRec( reader );
        return true;
    }

    Hier* root() {
        Hier<D> *ret = this;

        while ( ret->Father() != nullptr ) {
            ret = ret->Father();
        }

        return ret;
    }

    MatrixAnim& TRS() { return mTRS; }
    const MatrixAnim& TRS() const { return mTRS; }
    void TRS( const MatrixAnim& val ) { mTRS = val; }

    const JMATH::AABB BBox3d() const { return bbox3d; }

    inline bool isDescendantOf( const UUID& ancestorHash ) {
        if ( mHash == ancestorHash ) return true;

        auto f = father;
        while ( f ) {
            if ( f->mHash == ancestorHash ) return true;
            f = f->father;
        }

        return false;
    }

    template<typename TV>
	void visit() const {
        traverseWithHelper<TV>( "Name,GHType,Geom,BBbox,Children", mName,mGHType,mData,bbox3d,
                                children );
	}

protected:

    void getLocatorsPosRec( std::vector<Vector3f>& _locators ) {

        if ( checkBitWiseFlag( GHType(), GHTypeLocator )) {
            Vector3f lpos = mLocalHierTransform->getPosition3();
            _locators.push_back( lpos );
        }

        for ( auto& c : children ) {
            c->getLocatorsPosRec( _locators );
        }
    }

    int totalChildrenRec( int& numC ) const {
        numC++;

        for ( auto& c : children ) {
            c->totalChildrenRec( numC );
        }

        return numC;
    }

    int totalChildrenOfTypeRec( GeomHierType _gt, int& numC ) const {
        if ( mGHType & _gt ) numC++;

        for ( auto& c : children ) {
            c->totalChildrenOfTypeRec( _gt, numC );
        }

        return numC;
    }

    int totalChildrenWithGeomRec( int& numC ) const {
        if ( mData ) numC++;

        for ( auto& c : children ) {
            c->totalChildrenWithGeomRec( numC );
        }

        return numC;
    }

    void updateTransformRec( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot, const Vector3f& scale,
                        UpdateTypeFlag whatToUpdate ) {
        if ( nodeName.compare( mName ) == 0 ) {
            generateLocalTransformData( whatToUpdate & UpdateTypeFlag::Position ? pos : mTRS.Pos(),
                                        whatToUpdate & UpdateTypeFlag::Rotation ? Quaternion{rot} : mTRS.Rot(),
                                        whatToUpdate & UpdateTypeFlag::Scale ? scale : mTRS.Scale());
        }

        for ( auto& c : children ) {
            c->updateTransformRec( nodeName, pos, rot, scale, whatToUpdate );
        }
    }

    void serializeDependenciesRec( std::shared_ptr<SerializeBin> writer ) {

        if ( mData ) {
            mData->serializeDependencies( writer );
        }

        for ( const auto& c : Children()) {
            c->serializeDependenciesRec( writer );
        }
    }

    void serializeDependencies( std::shared_ptr<SerializeBin> writer ) {

        serializeDependenciesRec( writer );
        // End tag so we know there are no more dependencies
        writer->write( uint32_t(0) );
    }

    void serializeRec( std::shared_ptr<SerializeBin> writer ) {
        writer->write( mGHType );
        writer->write( mHash );
        writer->write( mName );
        writer->write( mLocalTransform );
        writer->write( bbox3d );
        int32_t hasData = mData ? 1 : 0;
        writer->write( hasData );
        if ( hasData == 1 ) mData->serialize( writer );

        auto numChildren = static_cast<int32_t>( Children().size());
        writer->write( numChildren );
        for ( auto&& c : Children()) {
            c->serializeRec( writer );
        }
    }

    void deserializeRec( std::shared_ptr<DeserializeBin> reader, Hier<D> *_father = nullptr ) {
        father = _father;
        reader->read( mGHType );
        reader->read( mHash );
        reader->read( mName );
        reader->read( mLocalTransform );
        reader->read( bbox3d );

        int32_t hasData = 0;
        reader->read( hasData );
        if ( hasData == 1 ) {
            mData = std::make_shared<D>(reader);
        }

        int32_t numChildren = 0;
        reader->read( numChildren );

        for ( int32_t t = 0; t < numChildren; t++ ) {
            std::shared_ptr<Hier> gg = std::make_shared<Hier>();
            addChildren( gg );
            gg->deserializeRec( reader, this );
        }
    }

    void createLocalHierMatrix( Matrix4f cmat ) {
        Matrix4f& lLHT = *mLocalHierTransform;
        lLHT = mLocalTransform * cmat;
//    if ( mData ) {
//        bbox3d = mData->BBox3d();
//        bbox3d.transform( lLHT );
//    }
    }

    void pruneRec() {
        for ( auto it = children.begin(); it != children.end(); ) {
            if ( (*it)->Children().empty() && !((*it)->Geom()) ) {
                it = children.erase( it );
            } else {
                (*it)->pruneRec();
                ++it;
            }
        }
    }
    void generateMatrixHierarchyRec( Matrix4f cmat ) {
        createLocalHierMatrix( cmat );

        for ( auto& c : children ) {
            c->generateMatrixHierarchyRec( *mLocalHierTransform );
        }
    }
    void findRecursive( const char *name, Hier *& foundObj ) {
        if ( strcmp( mName.c_str(), name ) == 0 ) {
            foundObj = this;
            return;
        }

        for ( auto& c : children ) {
            c->findRecursive( name, foundObj );
            if ( foundObj != nullptr ) break;
        }
    }
    void containingAABBRec( JMATH::AABB& _bbox ) const {
        if ( mData ) {
            auto cr = BBox3d();
            _bbox.merge( cr );
        }

        for ( auto& c : children ) {
            c->containingAABBRec( _bbox );
        }
    }
    JMATH::AABB calcCompleteBBox3dRec() {
        if ( mData ) {
            JMATH::AABB lBBox = mData->BBox3d();
            lBBox.transform( *mLocalHierTransform );
            bbox3d = lBBox;
        }

        for ( auto& c : children ) {
            bbox3d.merge( c->calcCompleteBBox3dRec() );
        }

        return bbox3d;
    }
    void BBox3d( const Vector3f& bmix, const Vector3f& bmax ) { bbox3d = JMATH::AABB( bmix, bmax ); }
    JMATH::AABB containingAABB() const {
        JMATH::AABB ret = BBox3d();
        containingAABBRec( ret );
        return ret;
    }
    void calcCompleteBBox3d() {
        bbox3d = calcCompleteBBox3dRec();
    }

protected:
    std::string mName;
    UUID mHash;
    Hier *father = nullptr;
    GeomHierType mGHType = GHTypeGeneric;
    Matrix4f mLocalTransform = Matrix4f::IDENTITY;
    std::shared_ptr<Matrix4f> mLocalHierTransform;
    MatrixAnim mTRS;
    JMATH::AABB bbox3d = JMATH::AABB::INVALID;

    std::shared_ptr<D> mData;

    std::vector<std::shared_ptr<Hier>> children;
};
