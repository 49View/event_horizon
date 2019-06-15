//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <memory>
#include <core/name_policy.hpp>
#include <core/math/matrix_anim.h>
#include <core/boxable.hpp>
#include <core/uuidable.hpp>
#include <core/serialization.hpp>

enum UpdateTypeFlag {
    Nothing =  0,
    Position = 1,
    Rotation = 1 << 1,
    Scale =    1 << 2
};

class TransformNodeData {
public:
    TransformNodeData() {
        mLocalHierTransform = std::make_shared<Matrix4f>(Matrix4f::MIDENTITY());
    }
protected:
    MatrixAnim mTRS;
    Matrix4f mLocalTransform = Matrix4f::IDENTITY;
    std::shared_ptr<Matrix4f> mLocalHierTransform;
};

struct PFC{};

template <typename T, typename B>
class RecursiveTransformation : public Boxable<B>,
                                public NamePolicy<>,
                                public UUIDable,
                                public TransformNodeData,
                                public std::enable_shared_from_this<RecursiveTransformation<T,B>> {
public:

    using NodeSP = std::shared_ptr<RecursiveTransformation<T, B>>;
    using NodeP = RecursiveTransformation<T, B>*;

    RESOURCE_CTORS(RecursiveTransformation);
    void bufferDecode( const unsigned char* rawData, size_t length ) {}

    RecursiveTransformation() = default;
    virtual ~RecursiveTransformation() = default;

    template <typename ...Args>
    explicit RecursiveTransformation( PFC _pfc, Args&& ... args ) {
        pushData( std::forward<Args>( args )...);
    }

    // This ctor is effectively a "clone"
    RecursiveTransformation( const RecursiveTransformation<T,B>& _source ) {
        addNodeRec( _source, nullptr );
    }
    explicit RecursiveTransformation( const RecursiveTransformation<T,B>& _source, NodeP _father  ) {
        addNodeRec( _source, _father );
    }
    // This ctor is effectively a "clone"
    explicit RecursiveTransformation( NodeSP _source ) {
        addNodeRec( *_source.get(), nullptr );
    }
    RecursiveTransformation( NodeSP _source, NodeP _father ) {
        addNodeRec( *_source.get(), _father );
    }
    explicit RecursiveTransformation( const std::string& _name ) : NamePolicy(_name) {}
    template <typename R>
    RecursiveTransformation( const Vector3f& pos, const R& rot, const Vector3f& scale ) {
        static_assert( std::is_same<R, Vector3f>::value || std::is_same<R, Quaternion>::value );
        generateLocalTransformData(pos, rot, scale);
        generateMatrixHierarchy(fatherRootTransform());
    }

    void visit( std::function<void(NodeSP)> f ) {
        f(this->shared_from_this());
        for ( auto& c : Children() ) {
            c->visit( f );
        }
    }

    inline void serialize( MegaWriter* visitor ) const {
        visitor->StartObject();
        visitor->serialize( "UUID", UUiD() );
        visitor->EndObject();
    }
	inline SerializableContainer serialize() const {
        MegaWriter mw; serialize(&mw); return mw.getSerializableContainer();
    }

    bool empty() const {
        return data.empty();
    }

    const T& Data( size_t _index = 0 ) const {
        return data[_index];
    }

    T& DataRef( size_t _index = 0 ) {
        return data[_index];
    }

    template <typename ...Args>
    void pushData( Args&&... args ) {
        data.emplace_back( T{std::forward<Args>( args )...} );
    }

    void updateAnim() {
        if ( mTRS.isAnimating() ) {
//            mLocalTransform = Matrix4f{ mTRS };
            generateMatrixHierarchy( fatherRootTransform() * Matrix4f{ mTRS } );
        }
    }

    void Father( NodeP val ) { father = val; }
    NodeP Father() { return father; }
    NodeP Father() const { return father; }
    Matrix4f fatherRootTransform() const {
        if ( father == nullptr ) return Matrix4f::IDENTITY;
        return *father->mLocalHierTransform.get();;
    }

    void prune() {
        pruneRec( this->shared_from_this() );
    }

    V3fa& PosAnim() {
        return mTRS.pos;
    }
    Quaterniona& RotAnim() {
        return mTRS.rot;
    }
    V3fa& ScaleAnim() {
        return mTRS.scale;
    }
    MatrixAnim& TRS() { return mTRS; }
    const MatrixAnim& TRS() const { return mTRS; }
    void TRS( const MatrixAnim& val ) { mTRS = val; }

//    void containingAABBRec( JMATH::AABB& _bbox ) const {
//        auto cr = Boxable::BBox3d();
//        _bbox.merge( cr );
//
//        for ( auto& c : children ) {
//            c->containingAABBRec( _bbox );
//        }
//    }
//
    JMATH::AABB calcCompleteBBox3dRec() {
        if constexpr ( std::is_same_v<JMATH::AABB, B> ) {
            this->BBox3d(AABB::INVALID);
            if ( !data.empty() ) {
                for ( const auto & bd : data ) {
                    this->BBox3d().merge( bd.BBox3d().transform( *mLocalHierTransform ) );
                }
            }

            for ( auto& c : children ) {
                this->bbox3d.merge( c->calcCompleteBBox3dRec() );
            }

            return this->bbox3d;
        } else {
            return AABB::INVALID;
        }
    }
//
//    JMATH::AABB containingAABB() const {
//        JMATH::AABB ret = Boxable::BBox3d();
//        containingAABBRec( ret );
//        return ret;
//    }
//
    void calcCompleteBBox3d() {
        this->bbox3d = calcCompleteBBox3dRec();
    }

    void createLocalHierMatrix( Matrix4f cmat ) {
        *mLocalHierTransform = mLocalTransform * cmat;
    }

    void setTag( uint64_t _tag ) {
        tag = _tag;
        for ( auto& c : children ) {
            c->setTag( _tag );
        }
    }

    void generateMatrixHierarchyRec( Matrix4f cmat ) {
        createLocalHierMatrix( cmat );
        for ( auto& c : children ) {
            c->generateMatrixHierarchyRec( *mLocalHierTransform.get() );
        }
    }

    void generateMatrixHierarchy( Matrix4f cmat = Matrix4f::IDENTITY ) {
        generateMatrixHierarchyRec( cmat );
        calcCompleteBBox3d();
    }

    void generateLocalTransformData( const Vector3f& pos, const Vector4f& rotAxis, const Vector3f& scale = Vector3f::ONE ) {
        MatrixAnim lTRS;
        lTRS.set( pos, rotAxis, scale );
        mLocalTransform = Matrix4f{ lTRS };
    }

    void generateLocalTransformData( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale = Vector3f::ONE ) {
        MatrixAnim lTRS;
        lTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ lTRS };
    }

    void generateLocalTransformData( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale=Vector3f::ONE ) {
        MatrixAnim lTRS;
        lTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ lTRS };
    }

    void updateTransformRec( const std::string& nodeName,
                             const Vector3f& pos,
                             const Vector3f& rot,
                             const Vector3f& scale,
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

    void updateTransform( const Vector3f& pos, const Vector4f& rot ) {
        generateLocalTransformData( pos, rot, mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateTransform( const Vector3f& pos ) {
        generateLocalTransformData( pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateTransform() {
//        generateLocalTransformData( mTRS.Pos(), mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform() * Matrix4f{mTRS} );
    }

    void updateExistingTransform( const Vector3f& pos, const Vector4f& rot, const Vector3f& scale ) {
        MatrixAnim lTRS;
        lTRS.set( pos, rot, scale );
//        auto mm = Matrix4f{ mTRS };
        mLocalTransform = mLocalTransform * lTRS;

        generateMatrixHierarchy( fatherRootTransform() );
    }

    void move( const Vector3f& pos ) {
        generateLocalTransformData( mTRS.Pos() + pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    NodeSP addChildren( NodeSP _node, const Vector3f& pos = Vector3f::ZERO,
                            const Vector3f& rot = Vector3f::ZERO,
                            const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        auto geom = _node;
        geom->Father( this );
        geom->updateTransform( pos, rot, scale );
        children.push_back( geom );
        return geom;
    }

    NodeSP addChildren( const std::string& _name ) {
        NodeSP node = std::make_shared<RecursiveTransformation<T,B>>();
        node->Father( this );
        node->Name( _name );
        node->updateTransform();
        children.push_back( node );
        return node;
    }

    template <typename R>
    NodeSP addChildren( const Vector3f& pos = Vector3f::ZERO,
                            const R& rot = R::ZERO,
                            const Vector3f& scale = Vector3f::ONE ) {
        static_assert( std::is_same<R, Vector3f>::value || std::is_same<R, Quaternion>::value );
        NodeSP node = std::make_shared<RecursiveTransformation<T,B>>(pos, rot, scale);
        node->Father( this->shared_from_this() );
        node->updateTransform( pos, rot, scale );
        children.push_back( node );
        return node;
    }

    std::vector<NodeSP>  Children() const { return children; }
    std::vector<NodeSP>& Children() { return children; }

    Matrix4f RootTransform() const { return mLocalTransform; }
    void RootTransform( const Matrix4f& val ) { mLocalTransform = val; }
    std::shared_ptr<Matrix4f> getLocalHierTransform() { return mLocalHierTransform; }
    Matrix4f LocalTransform() const { return mLocalTransform; }
    Matrix4f& LocalTransform() { return mLocalTransform; }
    void LocalTransform( const Matrix4f& m ) { mLocalTransform = m; }

    uint64_t Tag() const { return tag; }
private:
    void addNodeRec( const RecursiveTransformation<T,B>& _node, NodeP _father ) {
        cloneData( _node, _father );
        for ( const auto& c : _node.Children() ) {
            children.emplace_back( std::make_shared<RecursiveTransformation<T,B>>( c, this ) );
        }
    }

    void cloneData( const RecursiveTransformation<T,B>& _source, NodeP _father ) {
        assingNewUUID();
        father = _father;
        Name( _source.Name() );
        this->BBox3d( _source.BBox3d() );
        data = _source.data;
        _source.TRS().clone(mTRS);
        mLocalTransform = _source.mLocalTransform;
        mLocalHierTransform = std::make_shared<Matrix4f>(*_source.mLocalHierTransform.get());
    }

    bool pruneRec( NodeSP it ) {
        for ( auto it2 = it->Children().begin(); it2 != it->Children().end(); ) {
            if ( it->pruneRec( *it2 ) ) {
                it2 = it->Children().erase( it2 );
            } else {
                ++it2;
            }
        }
        return it->Children().empty() && it->empty();
    }

protected:
    NodeP father = nullptr;
    uint64_t tag = 0;
    std::vector<T> data;
    std::vector<NodeSP> children;
};
