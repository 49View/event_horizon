//
// Created by dado on 20/05/2020.
//

#include "collision_mesh.hpp"
#include <core/math/collision_detection.hpp>

CollisionElement::CollisionElement( const V2f& p1, const V2f& p2, const V2f& normal ) : p1(p1), p2(p2),
                                                                                        normal(normal) {}
float CollisionMesh::collisionDetection( const V2f& pos, float radius ) {

    float ret = -1.0f;
    if ( !bbox.contains(pos) ) {
        return 1.0f;
    }

    for ( const auto& group : collisionGroups ) {
        if ( group.bbox.contains(pos) ) {
            for ( const auto& room : group.collisionRooms ) {
                if ( room.bbox.contains(pos) ) {
                    for ( const auto& element : room.collisionCollisionElement ) {
                        float i = lineSegmentCircleIntersection( element.p1, element.p2, pos, radius );
                        if ( i >= 0.0f ) {
                            LOGRS("Camera at " << pos);
                            LOGRS("Collided with " << element.p1 << " / " << element.p2);
                            LOGRS("distance:" << i);
                            return i;
                        }
                    }
                }
            }
        }
    }

    lastKnownGoodPosition = {pos.x(), lastKnownGoodPosition.y(), pos.y()};
    return ret;
}

void CollisionMesh::setLastKnownGoodPosition( const V3f& _pos ) {
    lastKnownGoodPosition = _pos;
}

V3f CollisionMesh::getLastKnownGoodPosition() const {
    return lastKnownGoodPosition;
}
