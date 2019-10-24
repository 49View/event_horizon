//
// Created by Dado on 23/10/2019.
//

#pragma once

#include <core/name_policy.hpp>
#include <core/serialization.hpp>
#include <utility>

enum LightType {
    LightType_Invalid = 0,
    LightType_Directional = 1,
    LightType_Point = 2,
    LightType_Spotlight = 3
};

using LightTypeT = uint64_t;

class Light : public Keyable<> {
public:
    JSONSERIAL( Light, mKey, type, mesh, wattage, intensity, attenuation, color, dir, pos, beamAngle );

    Light( LightTypeT type, const std::string& mesh, const Vector3f& pos, float wattage, float intensity,
           const Vector3f& attenuation ) : type( type ), mesh( mesh ), pos( pos ), wattage( wattage ),
                                           intensity( intensity ), attenuation( attenuation ) {}

    LightTypeT  type = LightType_Invalid;
    std::string mesh{};
    Vector3f	pos = V3f::ZERO;
    float       wattage = 50.0f;
    float       intensity = 1.0f;
    Vector3f    attenuation = V3f::ONE;
    Vector3f    color = V3f::ONE;
    Vector3f	dir = V3f::UP_AXIS_NEG;
    float       beamAngle = 0.0f;
};



