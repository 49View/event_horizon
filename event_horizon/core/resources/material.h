#pragma once

#include <core/heterogeneous_map.hpp>
#include <core/name_policy.hpp>

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

class Material : public Keyable<> {
public:
    JSONSERIAL( Material, mKey, values );
    explicit Material( const std::string& _type );
    explicit Material( const std::string& _type, const std::string& _key );
    explicit Material( std::shared_ptr<HeterogeneousMap> _values );
    explicit Material( const Color4f & _color, const std::string& _type );

    [[nodiscard]] float getMetallicValue() const;
    void setMetallicValue( float _metallicValue );
    [[nodiscard]] float getRoughnessValue() const;
    void setRoughnessValue( float _roughnessValue );
    [[nodiscard]] float getAoValue() const;
    void setAoValue( float _aoValue );
    [[nodiscard]] float getOpacity() const;
    void setOpacity( float _opacityValue );
    [[nodiscard]] V3f getDiffuseColor() const;
    void setDiffuseColor( const V3f& _value );

//    bool isStreammable() const;
    [[nodiscard]] float translucency() const;

    [[nodiscard]] const std::shared_ptr<HeterogeneousMap> Values() const;
    std::shared_ptr<HeterogeneousMap> Values();
    void Values( std::shared_ptr<HeterogeneousMap> _values );

private:
    void makeValues(  const std::string& _type );
protected:
    std::shared_ptr<HeterogeneousMap> values;
};

