//
//  hier_geom.hpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#pragma once

#include "core/math/quaternion.h"
#include "core/soa_utils.h"
#include "core/observable.h"
#include "core/math/matrix_anim.h"
#include "geom_data.hpp"

class SerializeBin;
class DeserializeBin;

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

class HierGeom : public ObservableShared<HierGeom>, public std::enable_shared_from_this<HierGeom> {
public:
    HierGeom();
    explicit HierGeom( const std::string& _name );
    explicit HierGeom( std::vector<char>_data );
    explicit HierGeom( const Vector3f& pos, const Vector3f& rot = Vector3f::ZERO, const Vector3f& scale = Vector3f::ONE );
    explicit HierGeom( std::shared_ptr<GeomData> data, HierGeom *papa = nullptr );

    int64_t Hash() const { return mHash; }
    void Hash( const int64_t _hashToSet ) { mHash = _hashToSet; }
    std::string Name() const { return mName; }
    void Name( std::string val ) { mName = val; }
    GeomHierType GHType() const { return mGHType; }
    void GHType( GeomHierType val ) { mGHType |= val; }
    bool hasType( GeomHierType val ) const { return ( mGHType & val ) > 0; }
    void removeType( GeomHierType val ) { mGHType = mGHType & ~val; }

    void copyTRSDataFrom( const HierGeom *source );
    void copyTransformDataFrom( const HierGeom *source );
    void mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox );
    void commandReposition( const std::vector<std::string>& itr );
    std::shared_ptr<HierGeom> clone() const;
    std::shared_ptr<HierGeom> addChildren( std::shared_ptr<GeomData> data, const Vector3f& pos = Vector3f::ZERO,
                                           const Vector3f& rot = Vector3f::ZERO,
                                           const Vector3f& /*scale*/ = Vector3f::ONE, bool visible = true );
    void
    addChildren( std::shared_ptr<GeomData> data, std::shared_ptr<HierGeom> _child, const Vector3f& pos = Vector3f::ZERO,
                 const Vector3f& rot = Vector3f::ZERO, const Vector3f& /*scale*/ = Vector3f::ONE, bool visible = true );
    std::shared_ptr<HierGeom> addChildren( std::shared_ptr<HierGeom> data );
    std::shared_ptr<HierGeom> addChildren( const std::string& name );
    std::shared_ptr<HierGeom> addChildren( const Vector3f& pos = Vector3f::ZERO,
                                           const Vector3f& rot = Vector3f::ZERO,
                                           const Vector3f& scale = Vector3f::ONE );

    int totalChildren() const;
    int totalChildrenWithGeom() const;
    int totalChildrenOfType( GeomHierType _gt ) const;

    void generateMatrixHierarchy( Matrix4f cmat = Matrix4f::IDENTITY );
    void updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot, const Vector3f& scale,
                          UpdateTypeFlag whatToUpdate );

    void updateTransform( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot );
    void updateTransform( const std::string& nodeName, const Vector3f& val, UpdateTypeFlag whatToUpdate );

    void updateTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale );
    void updateTransform( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale );
    void updateTransform( const Vector3f& pos, const Vector3f& rot );
    void updateTransform( const Vector3f& pos );
    void updateTransform();

    void generateLocalTransformData( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale = Vector3f::ONE );
    void generateLocalTransformData( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale=Vector3f::ONE );

    void SHReceiver( bool _isReceiver ) { mSHReceiver = _isReceiver; }
    bool SHReceiver() const { return mSHReceiver; }

    void subscribeRec( std::shared_ptr<ObserverShared<HierGeom>> _val );

    HierGeom *find( const char *name );
    void eraseChildren( const char *name );

    void extractHier( std::vector<std::shared_ptr<HierGeom>>& geoms, const char *name, ExtractFlags ef );
    Matrix4f RootTransform() const { return mLocalTransform; }
    void RootTransform( const Matrix4f& val ) { mLocalTransform = val; }
    std::shared_ptr<GeomData> Geom() { return mData; }
    const std::shared_ptr<GeomData> Geom() const { return mData; }
    void Geom( std::shared_ptr<GeomData> val ) { mData = val; }
    std::vector<std::shared_ptr<HierGeom>> Children() const { return children; }
    std::vector<std::shared_ptr<HierGeom>>& Children() { return children; }

    int numVerts();

    void removeTypeRec( const GeomHierType gt );
    void getGeomOfType( GeomHierType gt, std::vector<HierGeom *>& ret );
    void getGeomWithName( const std::string& _name, std::vector<HierGeom *>& ret );
    void getGeomWithHash( int64_t _hash, HierGeom *& retG );

    void Father( HierGeom *val ) { father = val; }
    HierGeom *Father() { return father; }
    HierGeom *Father() const { return father; }
    std::shared_ptr<Matrix4f> getLocalHierTransform() { return mLocalHierTransform; }
    void updateExistingTransform( const Vector3f& pos, const Vector3f& rot, const Vector3f& scale );
    Matrix4f fatherRootTransform() const;
    Matrix4f LocalTransform() const { return mLocalTransform; }
    void LocalTransform( const Matrix4f& m ) { mLocalTransform = m; }

    Vector3f position() const;

    void removeChildrenWithHash( int64_t _hash );
    void removeChildrenWithType( GeomHierType gt );

    void prune();
    void relightSH( bool includeChildren = true );
    void finalize();
    void generateSOA();
    void gatherStats();

    std::vector<unsigned char> serialize();
    bool deserialize( std::shared_ptr<DeserializeBin>& reader );

    const std::shared_ptr<PosTexNorTanBinUV2Col3dStrip>& getSOAData() const { return mSOAData; }

    HierGeom *root();

    MatrixAnim& TRS() { return mTRS; }
    const MatrixAnim& TRS() const { return mTRS; }
    void TRS( const MatrixAnim& val ) { mTRS = val; }

    const AABB BBox3d() const { return bbox3d; }
    void BBox3d( const Vector3f& bmix, const Vector3f& bmax ) { bbox3d = AABB( bmix, bmax ); }
    AABB containingAABB() const;
    void calcCompleteBBox3d();

    bool CastShadows() const { return mCastShadows; }
    void CastShadows( bool val ) { mCastShadows = val; }

    inline bool isDescendantOf( const int64_t& ancestorHash );

    std::vector<Vector3f> getLocatorsPos();

    static const std::string entityGroup() {
        return "geom";
    }

    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( "Name,GHType,Geom,BBbox,Children", mName,mGHType,mData,bbox3d,
	        children ); }

private:
    void intersectLineRec( const Vector3f& linePos, const Vector3f& lineDir, bool& doesHit );
    void
    updateTransformRec( const std::string& nodeName, const Vector3f& pos, const Vector3f& rot, const Vector3f& scale,
                        UpdateTypeFlag whatToUpdate );
    void numVertsRec( int& currNumVerts );

    void getLocatorsPosRec( std::vector<Vector3f>& _locators );
    void serializeDependencies( std::shared_ptr<SerializeBin> writer );
    void serializeDependenciesRec( std::shared_ptr<SerializeBin> writer );
    void serializeRec( std::shared_ptr<SerializeBin> writer );
    void deserializeRec( std::shared_ptr<DeserializeBin> reader, HierGeom *_father = nullptr );
    void createLocalHierMatrix( Matrix4f cmat );
    void generateGeometryVP();

protected:

    void pruneRec();
    void generateMatrixHierarchyRec( Matrix4f cmat );
    void findRecursive( const char *name, HierGeom *& foundObj );
    int totalChildrenRec( int& start ) const;
    int totalChildrenWithGeomRec( int& numC ) const;
    int totalChildrenOfTypeRec( GeomHierType _gt, int& numC ) const;
    void containingAABBRec( AABB& _bbox ) const;
    AABB calcCompleteBBox3dRec();

protected:
    std::string mName;
    int64_t mHash;
    GeomHierType mGHType = GHTypeGeneric;

    HierGeom *father = nullptr;
    std::shared_ptr<GeomData> mData;
    std::shared_ptr<PosTexNorTanBinUV2Col3dStrip> mSOAData;

    Matrix4f mLocalTransform = Matrix4f::IDENTITY;
    std::shared_ptr<Matrix4f> mLocalHierTransform;

    MatrixAnim mTRS;

    AABB bbox3d = AABB::INVALID;
    bool mSHReceiver = false;
    bool mCastShadows = true;

    std::vector<std::shared_ptr<HierGeom>> children;

    static int64_t globalHierHash;
};
