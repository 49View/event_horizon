//
// Created by dado on 30/09/2020.
//

#pragma once

//
// Created by melonqi on 2018/7/25.
// Adapted by Dado 30/09/2020
//

#include <vector>
#include <cmath>
#include <ostream>
#include <core/math/vector2f.h>

struct MinAreaRect
{
    double width;
    double height;
    double area;
    double angle_width;
    double angle_height;
    V2f center;
    V2f corner;
    V2f vector_width;
    V2f vector_height;
    std::vector<V2f> rect;

    friend std::ostream& operator<<( std::ostream& os, const MinAreaRect& rect );
};

class RotatingCalipers
{
    struct MinAreaState
    {
        size_t bottom;
        size_t left;
        double height;
        double width;
        double base_a;
        double base_b;
        double area;
    };
public:
    //the distance of point to the line, which describes by start and end
    static double _distance(const V2f &start, const V2f &end, const V2f &point);
    static inline double cross(const V2f &O, const V2f &A, const V2f &B);
    static std::vector<V2f> convexHull(std::vector<V2f> p);
    static inline double area(const V2f &a, const V2f &b, const V2f &c);
    static inline double dist(const V2f &a, const V2f &b);
    [[maybe_unused]] static double diameter(const std::vector<V2f> &p);
    static MinAreaRect minAreaRect(const std::vector<V2f> &p);
};

