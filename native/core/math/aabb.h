#pragma once

#include "vector3f.h"
#include "matrix4f.h"
#include "rect2f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone

enum class CompositeWrapping {
    Wrap,
    NoWrap
};

namespace JMATH {
    enum class AABBFeature {
        invalid = 0,
    };

    class AABB {
    public:

        static AABB& MIDENTITY();
        static AABB& ZERO();
        static AABB& MIDENTITYCENTER();
        static AABB& MINVALID();

        AABB() = default;
        AABB( const Vector3f& minPoint, const Vector3f& maxPoint );
        AABB( const Vector3f& minPoint, const Vector3f& maxPoint, bool bt );
        explicit AABB( const std::vector<Vector3f>& points );

        static constexpr bool IsSerializable() { return true; }
        [[nodiscard]] bool isValid() const;
        float *rawPtr();

        friend std::ostream& operator<<( std::ostream& os, const AABB& f );
        bool operator==( const AABB& rhs ) const;
        bool operator!=( const AABB& rhs ) const;
        AABB operator-( const Vector3f& rhs ) const;
        AABB operator+( const Vector3f& rhs ) const;
        void operator-=( const Vector3f& rhs );
        void operator+=( const Vector3f& rhs );
        AABB operator*( const Vector3f& rhs ) const;


        void set( const AABB& _aabb );
        void set( const Vector3f& minPoint, const Vector3f& maxPoint );
        void setCenterAndSize( const Vector3f& _center, const Vector3f& _size );

        void calc( const JMATH::Rect2f& bbox, float minHeight, float maxHeight, const Matrix4f& tMat );
        void calc( const std::initializer_list<Vector3f>& iList, const Matrix4f& tMat );
        void calc( const std::vector<Vector3f>& vList, const Matrix4f& tMat );

        void expand( const Vector3f& p );
        void expandMin( const Vector3f& p );
        void expandMax( const Vector3f& p );

        void merge( const AABB& val );

        void identity();
        void identityCentered();

        [[nodiscard]] Rect2f demoteTo2d() const;
        [[nodiscard]] JMATH::Rect2f topDown() const;
        [[nodiscard]] JMATH::Rect2f front() const;
        [[nodiscard]] int leastDominantAxis() const;

        [[nodiscard]] float calcWidth() const;
        [[nodiscard]] float calcHeight() const;
        [[nodiscard]] float calcDepth() const;
        [[nodiscard]] float calcDiameter() const;
        [[nodiscard]] float calcRadius() const;
        [[nodiscard]] Vector3f calcCentre() const;

        [[nodiscard]] float pivotHeight() const;
        [[nodiscard]] float pivotWidth() const;

        [[nodiscard]] Vector3f centreTop() const;
        [[nodiscard]] Vector3f centreBottom() const;
        [[nodiscard]] Vector3f centreFront() const;
        [[nodiscard]] Vector3f centreBack() const;
        [[nodiscard]] Vector3f centreLeft() const;
        [[nodiscard]] Vector3f centreRight() const;

        [[nodiscard]] Vector3f bottomFront() const;
        [[nodiscard]] Vector3fList bottomFace() const;

        void translate( const Vector3f& offset );
        [[nodiscard]] AABB rotate( const Quaternion& axisAngle ) const;
        void scaleX( float x );
        void scaleY( float x );
        void scaleZ( float x );
        void scale( float x );

        void transform( const Matrix4f& mat );

        [[nodiscard]] AABB getTransform( const Matrix4f& mat ) const;
        [[nodiscard]] std::vector<Vector3f> topDownOutline( CompositeWrapping _wrap = CompositeWrapping::NoWrap ) const;

        bool intersectLine( const Vector3f& linePos, const Vector3f& lineDir, float& tNear, float& tFar ) const;
        [[maybe_unused]] [[nodiscard]] bool containsXZ( const V2f& _point ) const;
        [[nodiscard]] bool containsXY( const V2f& _point ) const;

        [[nodiscard]] bool containsX( float _value ) const;
        [[nodiscard]] bool containsY( float _value ) const;
        [[nodiscard]] bool containsZ( float _value ) const;

//	Vector3f minPoint() { return mMinPoint; }
//	Vector3f maxPoint() { return mMaxPoint; }

        [[nodiscard]] const Vector3f& minPoint() const;
        [[nodiscard]] const Vector3f& maxPoint() const;

        void setMinPoint( const Vector3f& val );
        void setMaxPoint( const Vector3f& val );

        [[nodiscard]] Vector3f size() const;
        [[nodiscard]] Vector3f centre() const;

    private:
        // DO NOT Change the order of these member variables as they are needed to get proper indices for raw access (performance requirement)
        Vector3f mMinPoint;
        Vector3f mMaxPoint;
    };
}
