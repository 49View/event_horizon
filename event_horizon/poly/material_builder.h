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

const std::vector<std::string>& pbrNames();

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

    MaterialBuilder( MaterialBuilder& a ) : ResourceBuilder(a.Name()),
              materialType( a.materialType ), shaderName( a.shaderName ), baseSolidColor( a.baseSolidColor ),
              metallicValue( a.metallicValue ), roughnessValue( a.roughnessValue ), aoValue( a.aoValue ),
              imageExt( a.imageExt ) {
        for ( auto&& [k,v] : a.buffers ) {
            buffers.emplace( std::make_pair( k, std::move( v ) ) );
        }
    }

    MaterialBuilder( const std::string& _name, MaterialType _mt, const std::string& _sn = "" );
    MaterialBuilder( const std::string& _name, MaterialType _mt, const std::string& _sn, const MaterialProperties& _p );
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

    MaterialBuilder& mt( const MaterialType _value ) {
        materialType = _value;
        return *this;
    }

    MaterialBuilder& mp( const MaterialProperties& _value ) {
        properties = _value;
        return *this;
    }

    MaterialBuilder& sh( const std::string& _value ) {
        shaderName = _value;
        return *this;
    }

    MaterialBuilder& bc( const Vector3f& _value ) {
        baseSolidColor = _value;
        return *this;
    }

    MaterialBuilder& c( const Color4f& _value ) {
        color = _value;
        return *this;
    }

    MaterialBuilder& mv( const float _value ) {
        metallicValue = _value;
        return *this;
    }
    MaterialBuilder& rv( const float _value ) {
        roughnessValue = _value;
        return *this;
    }
    MaterialBuilder& aov( const float _value ) {
        aoValue = _value;
        return *this;
    }

    MaterialBuilder& tn( const std::string& _value ) {
        textureName = _value;
        return *this;
    }

    MaterialBuilder& o( const float _value ) {
        opacity = _value;
        return *this;
    }

protected:
    const std::string pbrPrefix() const {
        return getFileNameNoExt( Name() ) + "_";
    }
    void handleUninitializedDefaults( DependencyMaker& _md, const std::string& _keyTextureName );
    void createDefaultPBRTextures( std::shared_ptr<PBRMaterial> mat, DependencyMaker& _md );
    std::string generateThumbnail() const;
    std::string generateRawData() const;
    std::string toMetaData() const;

private:
    MaterialType            materialType = MaterialType::PBR;
    MaterialProperties      properties;
    std::string             shaderName;
    Color4f                 color = Color4f::WHITE;
    Vector3f                baseSolidColor = Vector3f::ONE;
    float                   metallicValue = 1.0f;
    float                   roughnessValue = 1.0f;
    float                   aoValue = 1.0f;

    std::string             textureName = "white";
    float                   opacity = 1.0f;

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
