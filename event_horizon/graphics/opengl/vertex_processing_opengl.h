#pragma once

#include <algorithm>
#include <set>

#include <core/math/matrix4f.h>
#include <core/soa_utils.h>
#include <core/descriptors/uniform_names.h>

#include "../render_material.hpp"
#include "../texture_manager.h"
#include "program_opengl.h"

enum class RenderPass {
    ShadowMap,
    Reflection,
    Scene
};

enum class DisplayListType {
    DL_2D,
    DL_3D,
    DL_DV // Data visualization
};

struct cpuVertexDescriptor {
    int size;
    uintptr_t offset;
};

struct cpuVBIB {
    std::unique_ptr<int32_t[]> bufferIndices;
    std::unique_ptr<char[]> bufferVerts;
    int numVerts;
    int numIndices;
    Primitive primiteType;
    int elenentSize;

    cpuVertexDescriptor vElementAttrib[9];
    int vElementAttribSize;
};

class GPUVData {
public:
    void draw() const;
    void create( std::shared_ptr<cpuVBIB> vbib );

    bool Dynamic() const { return dynamic; }
    void Dynamic( bool val ) { dynamic = val; }

    void deleteBuffers() {
        glDeleteBuffers( 1, &vbo );
        glDeleteBuffers( 1, &ibo );
        glDeleteVertexArrays( 1, &vao );
    }

    inline bool isEmpty() const {
        return numIndices == 0 || vao == 0;
    }

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint numIndices = 0;
    Primitive primitveType = PRIMITIVE_TRIANGLE_STRIP;
    bool dynamic = false;
};

class VertexProcessing {
public:
    VertexProcessing() = default;

    VertexProcessing( const uint64_t _tag, std::shared_ptr<RenderMaterial> _mat ) {
        mTag = _tag;
        material = _mat;
    }

    inline std::shared_ptr<RenderMaterial> getMaterial() const {
        return material;
    }

    inline void setMaterial( std::shared_ptr<RenderMaterial> mp ) {
        material = mp;
    }

    template<typename T>
    inline void setMaterialConstant( const std::string& name, const T& val ) {
        material->setConstant( name, val );
    }

    template<typename T>
    inline void setMaterialGlobalConstant( const std::string& name, const T& val ) {
        material->setGlobalConstant( name, val );
    }

    template<typename T>
    inline void setMaterialBufferConstant( const std::string& ubo_name, const std::string& name, const T& val ) {
        material->setBufferConstant( ubo_name, name, val );
    }

    template<typename T>
    inline T getMaterialConstant( const std::string& name, T& value ) {
        material->Uniforms()->Values()->get( name, value );
        return value;
    }

    inline float transparencyValue() const {
        return material->TransparencyValue();
    }

    inline void setMaterialConstantAlpha( float alpha ) {
        material->setConstant( UniformNames::alpha, alpha );
    }

    inline void setMaterialConstantOpacity( float alpha ) {
        material->setConstant( UniformNames::opacity, alpha );
    }

    void renderProgram() {
        if ( gpuData.isEmpty()) return;
        programStart( material );
        programDraw();
    }

    void renderProgramWith( std::shared_ptr<RenderMaterial> _material ) const {
        if ( gpuData.isEmpty()) return;
        programStart( _material );
        programDraw();
    }

    void create( std::shared_ptr<cpuVBIB> vbib );

    uint64_t tag() const {
        return mTag;
    }

    void tag( const uint64_t tag ) {
        mTag = tag;
    }

    template<class V>
    static std::shared_ptr<cpuVBIB>
    create_cpuVBIB( std::shared_ptr<VertexStripIBVB<V>> vbib ) {
        std::shared_ptr<cpuVBIB> ret = std::make_shared<cpuVBIB>();

        ret->elenentSize = sizeof( V );
        ret->bufferVerts = std::make_unique<char[]>( ret->elenentSize * vbib->numVerts );
        std::memcpy( ret->bufferVerts.get(), vbib->verts.get(), ret->elenentSize * vbib->numVerts );
        ret->bufferIndices = std::move( vbib->indices );
        ret->numIndices = vbib->numIndices;
        ret->numVerts = vbib->numVerts;
        ret->primiteType = vbib->primiteType;
        ret->vElementAttribSize = V::numElements();
        for ( int32_t t = 0; t < V::numElements(); t++ ) {
            ret->vElementAttrib[t].offset = V::offset( t );
            ret->vElementAttrib[t].size = V::size( t );
        }
        return ret;
    }

private:
    void programStart( std::shared_ptr<RenderMaterial> _material ) const;
    void programDraw() const;

private:
    GPUVData gpuData;
    std::shared_ptr<RenderMaterial> material;
    uint64_t mTag = GT_Generic;
};
