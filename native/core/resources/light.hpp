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
JSONSERIAL(Light, mKey, type, mesh, wattage, intensity, attenuation, color, dir, pos,
           beamAngle);

    Light( LightTypeT type, const std::string& _key, const std::string& mesh, const Vector3f& pos,
           float wattage, float intensity, const Vector3f& attenuation ) : type(type), mKey(_key),
                                                                           mesh(mesh), pos(pos), wattage(wattage),
                                                                           intensity(intensity),
                                                                           attenuation(attenuation) {}

    LightTypeT type = LightType_Invalid;
    std::string mKey;
    std::string mesh{};
    Vector3f pos = V3fc::ZERO;
    float wattage = 50.0f;
    float intensity = 1.0f;
    Vector3f attenuation = V3fc::ONE;
    Vector3f color = V3fc::ONE;
    Vector3f dir = V3fc::UP_AXIS_NEG;
    float beamAngle = 0.0f;
};



