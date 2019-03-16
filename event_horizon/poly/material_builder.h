#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <functional>
#include "core/util.h"
#include "core/builders.hpp"
#include "core/descriptors/material.h"
#include "core/image_constants.h"

class MaterialBuilder;
struct MaterialProperties;
class ColorBuilder;
class ImageDepencencyMaker;

class MaterialManager : public DependencyMakerPolicy<Material> {
public:
    virtual ~MaterialManager() = default;
    void dump() const;
//    bool virtual add( const MaterialBuilder& pb, std::shared_ptr<Material> _material );
//    bool add( const std::string& _key, std::shared_ptr<Material> _material );
//    std::shared_ptr<Material> get( const std::string& _key, const std::string& _subkey, const MaterialProperties& _mp );
    std::vector<std::shared_ptr<Material>> list() const;
    void TL( ImageDepencencyMaker* _tl ) { tl = _tl; }
    ImageDepencencyMaker* TL() { return tl; }
private:
    ImageDepencencyMaker* tl = nullptr;
};

//struct RBUILDER( MaterialBuilder, material, mat, Binary, BuilderQueryType::NotExact,  )

class MaterialBuilder : public ResourceBuilder< Material, MaterialManager > {
    using ResourceBuilder::ResourceBuilder;
public:
//    MaterialBuilder( MaterialBuilder& a ) : ResourceBuilder(a) {}
    explicit MaterialBuilder( MaterialManager& _mm ) : ResourceBuilder(_mm) {}
    MaterialBuilder( MaterialManager& _mm, const std::string& _name, const std::string& _sn );
    MaterialBuilder( MaterialManager& _mm, const std::string& _name, const SerializableContainer& _data );

    void makeDefault() override;
    std::shared_ptr<Material> makeDirect();
private:
    MaterialBuilder& mp( const MaterialProperties& _value ) {
        properties = _value;
        return *this;
    }

    MaterialBuilder& sh( const std::string& _value ) {
        shaderName = _value;
        return *this;
    }

private:
    MaterialProperties      properties;
    std::string             shaderName = S::SH;

    SerializableContainer   bufferTarFiles;
    std::string imageExt = ".png";
};

using MB = MaterialBuilder;

// Color management!

class ColorManager : public DependencyMakerPolicy<MaterialColor> {
public:
    virtual ~ColorManager() = default;
};

class ColorBuilder : public ResourceBuilder<MaterialColor, ColorManager> {
    using ResourceBuilder::ResourceBuilder;
};

//struct RBUILDER( ColorBuilder, color, col, Text, BuilderQueryType::NotExact, MaterialColor::Version() )
//};
