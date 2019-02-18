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

struct MaterialBuilder;
struct MaterialProperties;
struct ColorBuilder;
class ImageDepencencyMaker;

class MaterialManager : public DependencyMaker {
public:
    DEPENDENCY_MAKER_EXIST(materialList);
    void dump() const;
    bool virtual add( const MaterialBuilder& pb, std::shared_ptr<Material> _material );
    bool add( std::shared_ptr<Material> _material );
    std::shared_ptr<Material> get( const std::string& _key );
    std::shared_ptr<Material> get( const std::string& _key, const std::string& _subkey, const MaterialProperties& _mp );
    std::vector<std::shared_ptr<Material>> list() const;
    void TL( ImageDepencencyMaker* _tl ) { tl = _tl; }
    ImageDepencencyMaker* TL() { return tl; }
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materialList;
    ImageDepencencyMaker* tl;
};

struct RBUILDER( MaterialBuilder, material, mat, Binary, BuilderQueryType::NotExact, Material::Version() )

    MaterialBuilder( MaterialBuilder& a ) : ResourceBuilder(a.Name()),shaderName( a.shaderName ), imageExt( a.imageExt ) {
        bufferTarFiles = a.bufferTarFiles;
    }

    MaterialBuilder( const std::string& _name, const std::string& _sn );
    MaterialBuilder( const std::string& _name, const std::vector<char>& _data );

    void makeDefault( DependencyMaker& _md );
    std::shared_ptr<Material> makeDirect( DependencyMaker& _md );

    MaterialBuilder& mp( const MaterialProperties& _value ) {
        properties = _value;
        return *this;
    }

    MaterialBuilder& sh( const std::string& _value ) {
        shaderName = _value;
        return *this;
    }

protected:
    const std::string pbrPrefix() const {
        return getFileNameNoExt( Name() ) + "_";
    }
    void handleUninitializedDefaults( DependencyMaker& _md, const std::string& _keyTextureName );
    void imageBuilderInjection( DependencyMaker& _md, const std::string& _finame, ucchar_p _dataPtr );
private:
    MaterialProperties      properties;
    std::string             shaderName = S::SH;

    std::vector<char>       bufferTarFiles;
    std::string imageExt = ".png";
};

// Color management!

class ColorManager : public DependencyMaker {
public:
DEPENDENCY_MAKER_EXIST(colorList);
    bool virtual add( const ColorBuilder& pb, const MaterialColor& _color );
    Color4f getColor( const std::string& _key );

private:
    std::unordered_map<std::string, MaterialColor> colorList;
};

struct RBUILDER( ColorBuilder, color, col, Text, BuilderQueryType::NotExact, MaterialColor::Version() )
};
