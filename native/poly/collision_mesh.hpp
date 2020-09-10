//
// Created by dado on 20/05/2020.
//

#pragma once

#include <vector>
#include <core/math/rect2f.h>

class CollisionElement {
public:
    V2f p1;
    V2f p2;
    V2f normal;

    CollisionElement( const V2f& p1, const V2f& p2, const V2f& normal );
};

class CollisionRoom {
public:
    std::vector<CollisionElement> collisionCollisionElement;
    Rect2f bbox;
};

class CollisionGroup {
public:
    std::vector<CollisionRoom> collisionRooms;
    Rect2f bbox;
};

class CollisionMesh {
public:
    float collisionDetection( const V3f& pos, float radius );
    void setLastKnownGoodPosition( const V3f& _pos );
    V3f getLastKnownGoodPosition() const;

protected:
    void resolveCollision( V2f& pos, float radius, int countRep, float& hitAccumulation, bool& hasGivenUp );

public:
    std::vector<CollisionGroup> collisionGroups;
    Rect2f bbox;
    V3f lastKnownGoodPosition = V3fc::HUGE_VALUE_NEG;
};



