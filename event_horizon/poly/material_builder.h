#pragma once

#include <memory>
#include <string>
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
        for ( auto&& [k,v] : a.buffers ) {
            buffers.emplace( std::make_pair( k, std::move( v ) ) );
        }
    }

    MaterialBuilder( const std::string& _name, const std::string& _sn );
    void makeDefault( DependencyMaker& _md );
    void makeDirect( DependencyMaker& _md );
    void publish() const;

    MaterialBuilder& buffer( const std::string& _bname, uint8_p&& _data ) {
        if ( _data.second > 0 ) {
            buffers.emplace( std::make_pair(pbrPrefix() + _bname, std::move(_data)) );
        }
        return *this;
    }

    MaterialBuilder& buffer( const std::string& _bname, const ucchar_p& _data ) {
        return buffer( _bname, ucchar_pTouint8_p(_data) );
    }

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
    std::string generateThumbnail() const;
    std::string generateRawData() const;
    std::string toMetaData() const;

private:
    MaterialProperties      properties;
    std::string             shaderName = S::SH;

    std::map<std::string, uint8_p> buffers;
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
