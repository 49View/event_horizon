#pragma once

#include <utility>
#include <unordered_set>

#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>

#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/names.hpp>
#include <core/name_policy.hpp>
#include <core/heterogeneous_map.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/image_util.h>
#include <core/util.h>
#include <core/math/vector4f.h>
#include <poly/resources/resource_utils.hpp>

using MaterialImageBuffers = std::unordered_map<std::string, uint8_p>;
using KnownBufferMap = std::unordered_map<std::string, std::string>;
using MaterialImageCallback = std::function<void( const std::string&, ucchar_p )>;

class MaterialColor {
public:
    JSONSERIAL( MaterialColor, color, name, category, brand, code, application );

    Vector4f color = Vector4f::WHITE;
    std::string name = "perfect white";
    std::string category = "white";
    std::string brand = "natural";
    std::string code = "255";
    std::string application = "matte";

    std::string toString() {
        return name + "\n" + color.toString() + "\n" + category + "\n" + brand + "\n" +
        code + "\n" + application + "\n";
    }
};

class Material {
public:
    JSONSERIAL( Material, values );
    explicit Material( const std::string& _type );

    void resolveDynamicConstants();

    float getMetallicValue() const;
    void setMetallicValue( float _metallicValue );
    float getRoughnessValue() const;
    void setRoughnessValue( float _roughnessValue );
    float getAoValue() const;
    void setAoValue( float _aoValue );
    float getOpacity() const;
    void setOpacity( float _opacityValue );

//    bool isStreammable() const;
    float translucency() const;

    const std::shared_ptr<HeterogeneousMap> Values() const;
    std::shared_ptr<HeterogeneousMap> Values();
    void Values( std::shared_ptr<HeterogeneousMap> _values );

protected:
    std::shared_ptr<HeterogeneousMap>     values;
};

