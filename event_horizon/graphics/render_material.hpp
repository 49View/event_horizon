//
//  material.hpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#pragma once

#include <string>
#include <unordered_map>
#include "core/descriptors/material.h"
#include "program_uniform_set.h"
#include "program_list.h"

class Program;

class ProgramUniformSet;

class ShaderManager;

class RenderMaterial {
public:
    MaterialType Type() const;
    std::string typeAsString();
    void Type( MaterialType val ) { mType = val; }
    std::shared_ptr<Program> BoundProgram() const { return boundProgram; }
    void BoundProgram( std::shared_ptr<Program> val );
    std::shared_ptr<ProgramUniformSet> Uniforms() { return uniforms; }
    std::shared_ptr<ProgramUniformSet> GlobalUniforms() { return globalUniforms; }
    std::shared_ptr<ProgramUniformSet> Constants() { return uniforms; }
    void Uniforms( std::shared_ptr<ProgramUniformSet> val ) { uniforms = val; }

    void submitBufferUniforms();
    void removeAllTextures();

    template<typename T>
    void setConstant( const std::string& _name, T value ) {
        uniforms->assign( _name, value );
        calcHash();
    }

    template<typename T>
    void setGlobalConstant( const std::string& _name, T value ) {
        globalUniforms->assignGlobal( _name, value );
    }

    template<typename T>
    void setGlobalConstant( const std::string& _name, std::shared_ptr<T> value ) {
        if ( value ) globalUniforms->assignGlobal( _name, *value.get());
    }

    template<typename T>
    void setBufferConstant( const std::string& ubo_name, const std::string& _name, T value ) {
        bufferUniforms[ubo_name]->setUBOData( _name, value );
    }

    inline int64_t Hash() const { return mHash; }
    float TransparencyValue() const { return mTransparencyValue; }
    void TransparencyValue( float val ) { mTransparencyValue = val; }

public:
    RenderMaterial( std::shared_ptr<Program> _program, std::shared_ptr<ProgramUniformSet> _uniforms );

private:
    void calcHash();

private:
    MaterialType mType;
    std::shared_ptr<Program> boundProgram;
    std::shared_ptr<ProgramUniformSet> uniforms;
    std::shared_ptr<ProgramUniformSet> globalUniforms;
    std::unordered_map<std::string, std::shared_ptr<ProgramUniformSet>> bufferUniforms;
    int64_t mHash;

    // These are calculated on the fly do not serialize
    float mTransparencyValue;
};

class Renderer;

class MaterialUniformRenderBaseSetup {
public:
    virtual void operator()( std::shared_ptr<Program> program, std::shared_ptr<ProgramUniformSet>& pus, Renderer& tub )
    const = 0;
};

class MaterialUniformRenderSetup : public MaterialUniformRenderBaseSetup {
public:
    MaterialUniformRenderSetup( std::shared_ptr<GenericMaterial> _material ) {
        material = _material;
    }

    virtual void operator()( std::shared_ptr<Program> program,
                             std::shared_ptr<ProgramUniformSet>& pus, Renderer& tub ) const;
protected:
    std::shared_ptr<GenericMaterial> material;
};

class MaterialPBRUniformRenderSetup : public MaterialUniformRenderBaseSetup {
public:
    MaterialPBRUniformRenderSetup( std::shared_ptr<PBRMaterial> _material ) {
        material = _material;
    }

    void operator()( std::shared_ptr<Program> program, std::shared_ptr<ProgramUniformSet>& pus,
                     Renderer& tub ) const;
protected:
    std::shared_ptr<PBRMaterial> material;
};

class RenderMaterialBuilder {
public:
    RenderMaterialBuilder( Renderer& rr ) : rr( rr ) {}

    RenderMaterialBuilder& p( const std::string& sn ) {
        shaderName = sn;
        return *this;
    }

    RenderMaterialBuilder& m( std::shared_ptr<Material> _material ) {
        material = _material;
        auto sn = material->getShaderName();
        shaderName = sn.empty() ? S::SH : sn;
        return *this;
    }

    RenderMaterialBuilder& m( const std::string& tn ) {
        doesNeedToAllocateBaseMaterial();
        material->setName( tn );
        return *this;
    }

    RenderMaterialBuilder& t( const std::string& tn ) {
        doesNeedToAllocateBaseMaterial();
        material->setTextureName( tn );
        return *this;
    }

    RenderMaterialBuilder& c( const Color4f& col ) {
        doesNeedToAllocateBaseMaterial();
        material->setColor( col );
        return *this;
    }

    std::shared_ptr<RenderMaterial> build();
private:
    void doesNeedToAllocateBaseMaterial() {
        if ( !material ) material = std::make_shared<GenericMaterial>();
    }

private:
    Renderer& rr;
    std::string shaderName;
    std::shared_ptr<Material> material;
};
