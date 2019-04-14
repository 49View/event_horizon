//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <memory>
#include <core/name_policy.hpp>
#include <core/math/matrix_anim.h>
#include <core/boxable.hpp>

enum UpdateTypeFlag {
    Nothing =  0,
    Position = 1,
    Rotation = 1 << 1,
    Scale =    1 << 2
};

template <typename T>
class RecursiveTransformation : public Boxable<JMATH::AABB>,
                                public NamePolicy<>,
                                public std::enable_shared_from_this<RecursiveTransformation<T>> {
public:

    using ChildrenSP = std::shared_ptr<RecursiveTransformation<T>>;

    RecursiveTransformation() = default;
    virtual ~RecursiveTransformation() = default;
    explicit RecursiveTransformation( const std::string& _name ) : NamePolicy(_name) {}
    template <typename R>
    RecursiveTransformation( const Vector3f& pos, const R& rot, const Vector3f& scale ) {
        static_assert( std::is_same<R, Vector3f>::value || std::is_same<R, Quaternion>::value );
        generateLocalTransformData(pos, rot, scale);
        generateMatrixHierarchy(fatherRootTransform());
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

    void pushData( const T& _data ) {
        data.emplace_back( _data );
    }

    void updateAnim() {
        if ( mTRS.isAnimating() ) {
            mLocalTransform = Matrix4f{ mTRS };
            generateMatrixHierarchy( fatherRootTransform());
        }
    }

    void Father( std::shared_ptr<RecursiveTransformation<T>> val ) { father = val; }
    std::shared_ptr<RecursiveTransformation<T>> Father() { return father; }
    std::shared_ptr<RecursiveTransformation<T>> Father() const { return father; }
    Matrix4f fatherRootTransform() const {
        if ( father == nullptr ) return Matrix4f::IDENTITY;
        return father->mLocalHierTransform;
    }

    void prune() {
        pruneRec( this->shared_from_this() );
    }

//    void containingAABBRec( JMATH::AABB& _bbox ) const {
//        auto cr = Boxable::BBox3d();
//        _bbox.merge( cr );
//
//        for ( auto& c : children ) {
//            c->containingAABBRec( _bbox );
//        }
//    }
//
//    JMATH::AABB calcCompleteBBox3dRec() {
//        auto lBBox = mData->BBox3d();
//        lBBox.transform( *mLocalHierTransform );
//        Boxable::bbox3d = lBBox;
//
//        for ( auto& c : children ) {
//            Boxable::bbox3d.merge( c->calcCompleteBBox3dRec() );
//        }
//
//        return Boxable::bbox3d;
//    }
//
//    JMATH::AABB containingAABB() const {
//        JMATH::AABB ret = Boxable::BBox3d();
//        containingAABBRec( ret );
//        return ret;
//    }
//
//    void calcCompleteBBox3d() {
//        Boxable::bbox3d = AABB::INVALID;
//        Boxable::bbox3d = calcCompleteBBox3dRec();
//    }

    void createLocalHierMatrix( Matrix4f cmat ) {
        mLocalHierTransform = mLocalTransform * cmat;
    }

    void generateMatrixHierarchyRec( Matrix4f cmat ) {
        createLocalHierMatrix( cmat );
        for ( auto& c : children ) {
            c->generateMatrixHierarchyRec( mLocalHierTransform );
        }
    }

    void generateMatrixHierarchy( Matrix4f cmat = Matrix4f::IDENTITY ) {
        generateMatrixHierarchyRec( cmat );
//        calcCompleteBBox3d();
    }

    void generateLocalTransformData( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale = Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
    }

    void generateLocalTransformData( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale=Vector3f::ONE ) {
        mTRS.set( pos, rot, scale );
        mLocalTransform = Matrix4f{ mTRS };
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

    void updateTransform( const Vector3f& pos ) {
        generateLocalTransformData( pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateTransform() {
        generateLocalTransformData( mTRS.Pos(), mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    void updateExistingTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale ) {
        mTRS.set( pos, rot, scale );
        auto mm = Matrix4f{ mTRS };
        mLocalTransform = mLocalTransform * mm;

        generateMatrixHierarchy( fatherRootTransform() );
    }

    void move( const Vector3f& pos ) {
        generateLocalTransformData( mTRS.Pos() + pos, mTRS.Rot(), mTRS.Scale());
        generateMatrixHierarchy( fatherRootTransform());
    }

    ChildrenSP addChildren( ChildrenSP _node, const Vector3f& pos = Vector3f::ZERO,
                            const Vector3f& rot = Vector3f::ZERO,
                            const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
        auto geom = _node;
        geom->Father( this->shared_from_this() );
        geom->updateTransform( pos, rot, scale );
        children.push_back( geom );
        return geom;
    }

//    void addChildren( std::shared_ptr<D> data, std::shared_ptr<Node<D>> _child, const Vector3f& pos = Vector3f::ZERO,
//                      const Vector3f& rot = Vector3f::ZERO, const Vector3f& scale = Vector3f::ONE, bool visible = true ) {
//        _child = std::make_shared<Node<D>>( data, this );
//        _child->updateTransform( pos, rot, scale );
//        children.push_back( _child );
//    }

//    ChildrenSP addChildren( ChildrenSP data ) {
//        data->Father( this );
//        data->updateTransform();
//        children.push_back( data );
//        return data;
//    }

    ChildrenSP addChildren( const std::string& _name ) {
        ChildrenSP node = std::make_shared<RecursiveTransformation<T>>();
        node->Father( this->shared_from_this() );
        node->Name( _name );
        node->updateTransform();
        children.push_back( node );
        return node;
    }

    template <typename R>
    ChildrenSP addChildren( const Vector3f& pos = Vector3f::ZERO,
                            const R& rot = R::ZERO,
                            const Vector3f& scale = Vector3f::ONE ) {
        static_assert( std::is_same<R, Vector3f>::value || std::is_same<R, Quaternion>::value );
        ChildrenSP node = std::make_shared<RecursiveTransformation<T>>(pos, rot, scale);
        node->Father( this->shared_from_this() );
        node->updateTransform( pos, rot, scale );
        children.push_back( node );
        return node;
    }

    std::vector<ChildrenSP>  Children() const { return children; }
    std::vector<ChildrenSP>& Children() { return children; }

    Matrix4f RootTransform() const { return mLocalTransform; }
    void RootTransform( const Matrix4f& val ) { mLocalTransform = val; }
    Matrix4f getLocalHierTransform() { return mLocalHierTransform; }
    Matrix4f LocalTransform() const { return mLocalTransform; }
    Matrix4f& LocalTransform() { return mLocalTransform; }
    void LocalTransform( const Matrix4f& m ) { mLocalTransform = m; }

private:
    bool pruneRec( ChildrenSP it ) {
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
    ChildrenSP father;
    MatrixAnim mTRS;
    Matrix4f mLocalTransform = Matrix4f::IDENTITY;
    Matrix4f mLocalHierTransform;
    std::vector<T> data;
    std::vector<ChildrenSP> children;
};
