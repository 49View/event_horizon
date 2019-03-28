#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <functional>
#include "core/util.h"
#include "poly/resources/resource_builder.hpp"
//#include "poly/material.h"
//#include "core/image_constants.h"

class Material;
class RawImage;

//class MaterialManager : public ResourceManager<Material> {
//public:
//    virtual ~MaterialManager() = default;
//    void dump() const;
////    bool virtual add( const MaterialBuilder& pb, std::shared_ptr<Material> _material );
////    bool add( const std::string& _key, std::shared_ptr<Material> _material );
////    std::shared_ptr<Material> get( const std::string& _key, const std::string& _subkey, const MaterialProperties& _mp );
//    std::vector<std::shared_ptr<Material>> list() const;
//    void TL( ImageDepencencyMaker* _tl ) { tl = _tl; }
//    ImageDepencencyMaker* TL() { return tl; }
//private:
//    ImageDepencencyMaker* tl = nullptr;
//};

//struct RBUILDER( MaterialBuilder, material, mat, Binary, BuilderQueryType::NotExact,  )

class MaterialBuilder : public ResourceBuilder2< MaterialBuilder, Material > {
public:
    using ResourceBuilder2::ResourceBuilder2;
//    MaterialBuilder( MaterialBuilder& a ) : ResourceBuilder(a) {}
//    explicit MaterialBuilder( MaterialManager& _mm ) : ResourceBuilder(_mm) {}
//    MaterialBuilder( const std::string& _sn );
//    MaterialBuilder( const SerializableContainer& _data );

private:
//    MaterialBuilder& mp( const MaterialProperties& _value ) {
//        properties = _value;
//        return *this;
//    }

    MaterialBuilder& sh( const std::string& _value ) {
        shaderName = _value;
        return *this;
    }

protected:
    void finalise( std::shared_ptr<Material> _elem ) override;

private:
//    MaterialProperties      properties;
    std::string             shaderName;

    SerializableContainer   bufferTarFiles;
    std::string imageExt = ".png";
};

using MB = MaterialBuilder;

// Color management!
class ColorBuilder : public ResourceBuilder2<ColorBuilder, MaterialColor> {
    using ResourceBuilder2::ResourceBuilder2;
};
