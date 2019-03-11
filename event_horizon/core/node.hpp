//
//  node.hpp
//
//  Created by Dado on 15/10/2196.
//
//

#pragma once

#include <core/math/quaternion.h>
#include <core/observable.h>
#include <core/math/matrix_anim.h>
#include <core/math/rect2f.h>
#include <core/math/aabb.h>
#include <core/math/anim.h>
#include <core/uuid.hpp>
#include <core/boxable.hpp>
#include <core/name_policy.hpp>
#include <core/publisher.hpp>
#include <core/observable.h>
#include <core/serialization.hpp>
#include <core/serializebin.hpp>
#include <core/zlib_util.h>
#include <core/http/basen.hpp>
#include <core/http/webclient.h>

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

enum class SerializeOutputFormat {
    Original,
    B64,
    GZip,
    B64GZip
};

inline constexpr static uint64_t NodeVersion( const uint64_t dataVersion ) { return (2040 * 1000000) + dataVersion; }

auto lambdaUpdateAnimVisitor = [](auto&& arg) { return arg->updateAnim();};
auto lambdaUpdateNodeTransform = [](auto&& arg) { arg->updateTransform();};

#define VisitLambda(X,Y) std::visit( [](auto&& arg) { arg-> X(); }, Y );

auto lambdaUUID = [](auto&& arg) -> UUID { return arg->UUiD();};
auto lambdaBBox = [](auto&& arg) -> JMATH::AABB { return arg->BBox3d();};

template <typename D>
class Node : public Animable,
             public ObservableShared<Node<D>>,
             public virtual Boxable<JMATH::AABB>,
             public virtual Publisher<D>,
             public std::enable_shared_from_this<Node<D>> {
public:
    Node() {
        mUUID = UUIDGen::make();
        mLocalHierTransform = std::make_shared<Matrix4f>(Matrix4f::IDENTITY);
    }
    virtual ~Node() = default;

    explicit Node( const std::string& _name ) : Node() {
        this->Name(_name);
    }
    explicit Node( std::shared_ptr<D> data, Node *papa = nullptr ) : Node() {
        this->Name( data->Name() );
        father = papa;
        mData = data;
    }
    explicit Node( std::shared_ptr<D> data, std::shared_ptr<Matrix4f> _clonedLocalTransform ) : Node() {
        mLocalHierTransform = _clonedLocalTransform;
        mData = data;
    }
    Node( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) : Node() {
        generateLocalTransformData(pos, rot, scale);
        generateMatrixHierarchy(fatherRootTransform());
    }

    explicit Node( const SerializableContainer& _data ) {
        auto reader = std::make_shared<DeserializeBin>( _data, D::Version() );
        deserializeImpl();
    }

    TimelineSet addKeyFrame( const std::string& _name, float _time ) override {
        TimelineSet ret{};

        ret.emplace( Timeline::add( _name, mTRS.pos  , {_time, mTRS.Pos()} ) );
        ret.emplace( Timeline::add( _name, mTRS.rot  , {_time, mTRS.Rot()} ) );
        ret.emplace( Timeline::add( _name, mTRS.scale, {_time, mTRS.Scale()} ) );

        return ret;
    }

    UUID UUiD() const { return mUUID; }
    NodeType GHType() const { return mGHType; }
    void GHType( NodeType val ) { mGHType |= val; }
    bool hasType( NodeType val ) const { return ( mGHType & val ) > 0; }
    void removeType( NodeType val ) { mGHType = mGHType & ~val; }

    template<typename F>
    void visitHashRecF( F _func ) {
        _func( UUiD() );
        for ( auto& c : Children()) {
            c->visitHashRecF( _func );
        }
    }

    template<typename F>
    void visitFixUp( F _func ) {
        _func( mData );
        for ( auto& c : Children()) {
            c->visitFixUp( _func );
        }
    }

    void sendNotifyData( const std::string& _event ) {
        if ( mData ) {
            this->notify( this->shared_from_this(), _event );
        }

        for ( auto& c : Children()) {
            c->sendNotifyData( _event );
        }
    }
    void subscribeData( std::shared_ptr<ObserverShared<Node<D>>> _val ) {
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

    int totalChildrenOfType( NodeType _gt ) const {
        int numC = 0;
        return totalChildrenOfTypeRec( _gt, numC );
    }

    void copyTRSDataFrom( const Node *source ) {
        generateLocalTransformData( source->TRS().pos->value, source->TRS().rot->value, source->TRS().scale->value );
    }
    void copyTransformDataFrom( const Node *source ) {
        copyTRSDataFrom( source );
    }
//    void commandReposition( const std::vector<std::string>& itr );

    std::shared_ptr<Node<D>> clone() {
        return std::make_shared<Node<D>>( *this );
    }

    std::shared_ptr<Node<D>> addChildren( std::shared_ptr<D> data, const Vector3f& pos = Vector3f::ZERO,
                                       const Vector3f& rot = Vector3f::ZERO,
                                       const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        auto geom = std::make_shared<Node<D>>( data, this );
        geom->updateTransform( pos, rot, scale );
        children.push_back( geom );
        return geom;
    }

    void addChildren( std::shared_ptr<D> data, std::shared_ptr<Node<D>> _child, const Vector3f& pos = Vector3f::ZERO,
                      const Vector3f& rot = Vector3f::ZERO, const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        _child = std::make_shared<Node<D>>( data, this );
        _child->updateTransform( pos, rot, scale );
        children.push_back( _child );
    }

    std::shared_ptr<Node<D>> addChildren( std::shared_ptr<Node<D>> data ) {
        data->Father( this );
        data->updateTransform();
        children.push_back( data );
        return data;
    }
    std::shared_ptr<Node<D>> addChildren( const std::string& name ) {
        std::shared_ptr<Node<D>> data = std::make_shared<Node<D>>();
        data->Father( this );
        data->Name( name );
        data->updateTransform();
        children.push_back( data );
        return data;
    }
    std::shared_ptr<Node<D>> addChildren( const Vector3f& pos = Vector3f::ZERO,
                                          const Vector3f& rot = Vector3f::ZERO,
                                          const Vector3f& scale = Vector3f::ONE ) {
        std::shared_ptr<Node<D>> data = std::make_shared<Node<D>>(pos, rot, scale);
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
    void move( const Vector3f& pos ) {
        generateLocalTransformData( mTRS.Pos() + pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateTransform() {
        generateLocalTransformData( mTRS.Pos(), mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }
    void updateAnim() {
        if ( mTRS.isAnimating() ) {
            mLocalTransform = Matrix4f{ mTRS };
            generateMatrixHierarchy( fatherRootTransform());
        }
    }

    void generateLocalTransformData( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale = Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
    }
    void generateLocalTransformData( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale=Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
    }

    Node* find( const char *name ) {
        Node *foundObj = nullptr;
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
    std::vector<std::shared_ptr<Node>> Children() const { return children; }
    std::vector<std::shared_ptr<Node>>& Children() { return children; }

    std::shared_ptr<D> Data() { return mData; }
    const std::shared_ptr<D> Data() const { return mData; }
    void Data( std::shared_ptr<D> val ) { mData = val; }

    void removeTypeRec( NodeType gt ) {
        removeType( gt );

        // Traverse tree
        for ( auto& c : children ) {
            c->removeTypeRec( gt );
        }
    }
    void getGeomOfType( NodeType gt, std::vector<Node *>& ret ) {
        if ( checkBitWiseFlag( GHType(), gt )) {
            ret.push_back( this );
        }

        // Traverse tree
        for ( auto& c : children ) {
            c->getGeomOfType( gt, ret );
        }
    }
    void getGeomWithName( const std::string& _name, std::vector<Node *>& ret ) {
        if ( this->Name() == _name ) {
            ret.push_back( this );
        }

        // Traverse tree
        for ( auto& c : children ) {
            c->getGeomWithName( _name, ret );
        }
    }
    void getGeomWithHash( const UUID& _hash, Node *& retG ) {
        bool stopRec = false;
        if ( UUiD() == _hash ) {
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

    void Father( Node *val ) { father = val; }
    Node *Father() { return father; }
    Node *Father() const { return father; }
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
    Matrix4f& LocalTransform() { return mLocalTransform; }
    void LocalTransform( const Matrix4f& m ) { mLocalTransform = m; }

    Vector3f position() const {
        return mLocalHierTransform->getPosition3();
    }

    void removeChildrenWithHash( const UUID& _hash ) {
        children.erase( remove_if( children.begin(), children.end(),
                                   [_hash]( std::shared_ptr<Node> const& us ) -> bool { return us->Hash() == _hash; } ),
                        children.end());

        // Traverse tree
        for ( auto& c : children ) {
            c->removeChildrenWithHash( _hash );
        }
    }

    void removeChildrenWithType( NodeType gt ) {
        children.erase( remove_if( children.begin(), children.end(), [gt]( std::shared_ptr<Node> const& us ) -> bool {
            return checkBitWiseFlag( us->GHType(), gt );
        } ), children.end());

        // Traverse tree
        for ( auto& c : children ) {
            c->removeChildrenWithType( gt );
        }
    }

    void prune() {
        pruneRec( this->shared_from_this() );
    }

    void finalize() {
        createLocalHierMatrix(fatherRootTransform());
        for ( auto& c : Children()) {
            c->finalize();
        }
    }

    Node* root() {
        Node<D> *ret = this;

        while ( ret->Father() != nullptr ) {
            ret = ret->Father();
        }

        return ret;
    }

    V3fa PosAnim() {
        return mTRS.pos;
    }
    Quaterniona RotAnim() {
        return mTRS.rot;
    }
    V3fa ScaleAnim() {
        return mTRS.scale;
    }
    MatrixAnim& TRS() { return mTRS; }
    const MatrixAnim& TRS() const { return mTRS; }
    void TRS( const MatrixAnim& val ) { mTRS = val; }

    inline bool isDescendantOf( const UUID& ancestorHash ) {
        if ( mUUID == ancestorHash ) return true;

        auto f = father;
        while ( f ) {
            if ( f->mUUID == ancestorHash ) return true;
            f = f->father;
        }

        return false;
    }

    template<typename TV>
    void visit() const {
        traverseWithHelper<TV>( "Name,Hash,BBbox,Data,Children", this->Name(), this->Hash(), Boxable::BBox3d(),mData,children );
    }
protected:

    void getLocatorsPosRec( std::vector<Vector3f>& _locators ) {

        if ( checkBitWiseFlag( GHType(), NodeTypeLocator )) {
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

    int totalChildrenOfTypeRec( NodeType _gt, int& numC ) const {
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
        if ( nodeName.compare( this->Name() ) == 0 ) {
            generateLocalTransformData( whatToUpdate & UpdateTypeFlag::Position ? pos : mTRS.Pos(),
                                        whatToUpdate & UpdateTypeFlag::Rotation ? Quaternion{rot} : mTRS.Rot(),
                                        whatToUpdate & UpdateTypeFlag::Scale ? scale : mTRS.Scale());
        }

        for ( auto& c : children ) {
            c->updateTransformRec( nodeName, pos, rot, scale, whatToUpdate );
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

    bool pruneRec( std::shared_ptr<Node> it ) {
        for ( auto it2 = it->Children().begin(); it2 != it->Children().end(); ) {
            if ( it->pruneRec( *it2 ) ) {
                it2 = it->Children().erase( it2 );
            } else {
                ++it2;
            }
        }
        return it->Children().empty() && !it->Data();
    }

    void generateMatrixHierarchyRec( Matrix4f cmat ) {
        createLocalHierMatrix( cmat );

        for ( auto& c : children ) {
            c->generateMatrixHierarchyRec( *mLocalHierTransform );
        }
    }

    void findRecursive( const char *name, Node *& foundObj ) {
        if ( strcmp( this->Name().c_str(), name ) == 0 ) {
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
            auto cr = Boxable::BBox3d();
            _bbox.merge( cr );
        }

        for ( auto& c : children ) {
            c->containingAABBRec( _bbox );
        }
    }

    JMATH::AABB calcCompleteBBox3dRec() {
        if ( mData ) {
            auto lBBox = mData->BBox3d();
            lBBox.transform( *mLocalHierTransform );
            Boxable::bbox3d = lBBox;
        }

        for ( auto& c : children ) {
            Boxable::bbox3d.merge( c->calcCompleteBBox3dRec() );
        }

        return Boxable::bbox3d;
    }

    JMATH::AABB containingAABB() const {
        JMATH::AABB ret = Boxable::BBox3d();
        containingAABBRec( ret );
        return ret;
    }

    void calcCompleteBBox3d() {
        Boxable::bbox3d = AABB::INVALID;
        Boxable::bbox3d = calcCompleteBBox3dRec();
    }

    std::string generateThumbnail() const override {
        return std::string{};
    }

    std::string calcHashImpl() override {
        std::stringstream ss;
        ss << UUiD();
        return ss.str();
    }

    void serializeDependenciesImpl( std::shared_ptr<SerializeBin> writer ) const override {
        if ( mData ) {
            mData->serializeDependenciesImpl( writer );
        }
        writer->writeDep( mData );

        for ( const auto& c : Children()) {
            c->serializeDependenciesImpl( writer );
        }
    }

    void deserializeImpl(std::shared_ptr<DeserializeBin> reader) override {
        deserializeRec( reader );
    }

private:
    void deserializeRec( std::shared_ptr<DeserializeBin> reader, Node<D> *_father = nullptr ) {
        father = _father;
        reader->read( mGHType );
        reader->read( mUUID );
        reader->read( this->NameRef() );
        mLocalHierTransform = std::make_shared<Matrix4f>(Matrix4f::IDENTITY);
        reader->read( mLocalTransform );
        reader->read( BBox3d() );

        reader->read( mData );

        int32_t numChildren = 0;
        reader->read( numChildren );

        for ( int32_t t = 0; t < numChildren; t++ ) {
            std::shared_ptr<Node> gg = std::make_shared<Node>();
            addChildren( gg );
            gg->deserializeRec( reader, this );
        }
    }

    void serializeImpl( std::shared_ptr<SerializeBin> writer ) const override {
        writer->write( mGHType );
        writer->write( mUUID );
        writer->write( this->Name() );
        writer->write( mLocalTransform );
        writer->write( Boxable::BBox3d() );

        writer->write( mData );

        auto numChildren = static_cast<int32_t>( Children().size() );
        writer->write( numChildren );
        for ( auto&& c : Children()) {
            c->serializeImpl( writer );
        }
    }

protected:
    UUID mUUID;
    Node *father = nullptr;
    NodeType mGHType = NodeTypeGeneric;
    Matrix4f mLocalTransform = Matrix4f::IDENTITY;
    std::shared_ptr<Matrix4f> mLocalHierTransform;
    MatrixAnim mTRS;

    std::shared_ptr<D> mData;
    std::vector<std::shared_ptr<Node>> children;
};
