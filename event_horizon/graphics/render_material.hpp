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

class Program;

class ProgramUniformSet;

class ShaderManager;

class RenderMaterial {
public:
    RenderMaterial( std::shared_ptr<Program> _program, std::shared_ptr<Material> _material, Renderer& _rr );

    std::shared_ptr<Program> BoundProgram() const { return boundProgram; }
    void BoundProgram( std::shared_ptr<Program> val );
    std::shared_ptr<ProgramUniformSet> Uniforms() { return uniforms; }
    std::shared_ptr<ProgramUniformSet> GlobalUniforms() { return globalUniforms; }
    std::shared_ptr<ProgramUniformSet> Constants() { return uniforms; }
    void Uniforms( std::shared_ptr<ProgramUniformSet> val ) { uniforms = val; }

    void submitBufferUniforms();
    void removeAllTextures();

    template<typename T>
    void setConstant( const std::string& _name, const T& value ) {
        uniforms->assign( _name, value );
        calcHash();
    }

    template<typename T>
    void setGlobalConstant( const std::string& _name, T value ) {
        globalUniforms->assign( _name, value );
    }

    template<typename T>
    void setGlobalConstant( const std::string& _name, std::shared_ptr<T> value ) {
        if ( value ) globalUniforms->assign( _name, *value.get());
    }

    template<typename T>
    void setBufferConstant( const std::string& ubo_name, const std::string& _name, T value ) {
        bufferUniforms[ubo_name]->setUBOData( _name, value );
    }

    inline int64_t Hash() const { return mHash; }
    float TransparencyValue() const { return mTransparencyValue; }
    void TransparencyValue( float val ) { mTransparencyValue = val; }

private:
    void calcHash();

private:
    std::shared_ptr<Material> sourceMaterial;
    std::shared_ptr<Program> boundProgram;
    std::shared_ptr<ProgramUniformSet> uniforms;
    std::shared_ptr<ProgramUniformSet> globalUniforms;
    std::unordered_map<std::string, std::shared_ptr<ProgramUniformSet>> bufferUniforms;
    int64_t mHash;

    Renderer& rr;
    // These are calculated on the fly do not serialize
    float mTransparencyValue;
};
