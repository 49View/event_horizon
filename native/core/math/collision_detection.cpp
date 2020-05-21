//
// Created by dado on 20/05/2020.
//

#include "collision_detection.hpp"

#include <core/math/vector2f.h>

float lineSegmentCircleIntersection( const V2f& p1, const V2f& p2, const V2f& center, float radius ) {

    V2f d = p2-p1;
    V2f f = p1 - center;// ( Vector from center sphere to ray start )

    float a = dot(d, d ) ;
    float b = 2.0f*dot(f, d ) ;
    float c = dot(f, f ) - radius*radius;

    float discriminant = b*b-4*a*c;
    if( discriminant < 0.0f )
    {
        return -1.0f;
        // no intersection
    }
    else
    {
        // ray didn't totally miss sphere,
        // so there is a solution to
        // the equation.

        discriminant = sqrt( discriminant );

        // either solution may be on or off the ray so need to test both
        // t1 is always the smaller value, because BOTH discriminant and
        // a are nonnegative.
        float t1 = (-b - discriminant)/(2.0f*a);
        float t2 = (-b + discriminant)/(2.0f*a);

        // 3x HIT cases:
        //          -o->             --|-->  |            |  --|->
        // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

        // 3x MISS cases:
        //       ->  o                     o ->              | -> |
        // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

        if( t1 >= 0.0f && t1 <= 1.0f )
        {
            // t1 is the intersection, and it's closer than t2
            // (since t1 uses -b - discriminant)
            // Impale, Poke
            return t1;
        }

        // here t1 didn't intersect so we are either started
        // inside the sphere or completely past it
        if( t2 >= 0.0f && t2 <= 1.0f )
        {
            // ExitWound
            return t2;
        }

        // no intn: FallShort, Past, CompletelyInside
        return -1.0f;
    }
}

