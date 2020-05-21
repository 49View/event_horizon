//
// Created by dado on 20/05/2020.
//

#pragma once

using V2f = class Vector2f;

float lineSegmentCircleIntersection( const V2f& p1, const V2f& p2, const V2f& center, float radius );
float lineSegmentCapsuleIntersection( const V2f& p1, const V2f& p2, const V2f& center1, const V2f& center2, float radius );


