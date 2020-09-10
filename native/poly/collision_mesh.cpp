//
// Created by dado on 20/05/2020.
//

#include "collision_mesh.hpp"
#include <core/math/collision_detection.hpp>

CollisionElement::CollisionElement( const V2f& p1, const V2f& p2, const V2f& normal ) : p1(p1), p2(p2),
                                                                                        normal(normal) {}

void CollisionMesh::resolveCollision( V2f& pos, float radius, int repCount, float& hitAccumulation, bool& hasGivenUp ) {
    for ( const auto& group : collisionGroups ) {
        if ( group.bbox.contains(pos) ) {
            for ( const auto& room : group.collisionRooms ) {
                if ( room.bbox.contains(pos) ) {
                    for ( const auto& element : room.collisionCollisionElement ) {
//                        bool i = lineSegmentCapsuleIntersection( element.p1, element.p2, pos, XZY::C2(lastKnownGoodPosition), radius);
                        float i = lineSegmentCircleIntersection( element.p1, element.p2, pos, radius );
                        if ( i >= 0.0f ) {
                            if ( repCount > 50 ) {
                                hasGivenUp = true;
                                return;
                            }
                            V2f pointOfContact = lerp(i, element.p1, element.p2);
                            float hitDistance = (radius - distance(pointOfContact, pos));
                            hitAccumulation+= hitDistance;
//                            LOGRS("Hit number: " << repCount << " Camera at " << pos);
//                            LOGRS("Collided with " << element.p1 << " / " << element.p2);
//                            LOGRS("distance:" << hitDistance);
                            pos += element.normal * (hitDistance + 0.001f);
                            resolveCollision(pos, radius, ++repCount, hitAccumulation, hasGivenUp );
                        }
                    }
                }
            }
        }
    }
}

float CollisionMesh::collisionDetection( const V3f& pos, float radius ) {
    V2f newPos = XZY::C2(pos);
    float hitAccumulation = 0.0f;
    bool hasGivenUp = false;

    if ( lastKnownGoodPosition == V3fc::HUGE_VALUE_NEG ) {
        lastKnownGoodPosition = pos;
        return hitAccumulation;
    }

    resolveCollision(newPos, radius, 0, hitAccumulation, hasGivenUp);
    if ( !hasGivenUp ) {
        lastKnownGoodPosition = {newPos.x(), pos.y(), newPos.y()};
    }
    return hitAccumulation;
}

void CollisionMesh::setLastKnownGoodPosition( const V3f& _pos ) {
    lastKnownGoodPosition = _pos;
}

V3f CollisionMesh::getLastKnownGoodPosition() const {
    return lastKnownGoodPosition;
}
