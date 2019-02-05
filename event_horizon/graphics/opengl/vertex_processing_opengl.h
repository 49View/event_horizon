#pragma once

#include <algorithm>
#include <set>

#include "core/math/matrix4f.h"
#include "core/soa_utils.h"

#include "../render_material.hpp"
#include "../program_uniform_set.h"
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
    std::shared_ptr<RenderMaterial> material;
    std::string name;

    cpuVertexDescriptor vElementAttrib[9];
    int vElementAttribSize;
};

class VertexProcessing {
public:
    VertexProcessing() = default;

    VertexProcessing( const std::string& _name, const uint64_t _tag, std::shared_ptr<RenderMaterial> _mat ) {
        Name( _name );
        mTag = _tag;
        material = _mat;
    }

    bool hasData() const {
        return vao != 0;
    }

    GLenum VFAttribTypeToGL( VFAttribTypes source ) {
        switch ( source ) {
            case VFAttribTypes::Byte:
                return GL_BYTE;
            case VFAttribTypes::UnsignedByte:
                return GL_UNSIGNED_BYTE;
            case VFAttribTypes::Short:
                return GL_SHORT;
            case VFAttribTypes::UnsignedShort:
                return GL_UNSIGNED_SHORT;
            case VFAttribTypes::Int:
                return GL_INT;
            case VFAttribTypes::UnsignedInt:
                return GL_UNSIGNED_INT;
            case VFAttribTypes::Float:
                return GL_FLOAT;
            default:
                return GL_FLOAT;
        }
        return GL_FLOAT;
    }

    inline int64_t hash() const {
        return material->Hash();
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
        material->Uniforms()->get( name, value );
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

    bool isEmpty() const {
        return numIndices == 0 || vao == 0;
    }

    void renderProgram() {
        if ( isEmpty()) return;
        programStart( material );
        programDraw();
    }

    void renderProgramWith( std::shared_ptr<RenderMaterial> _material ) const {
        if ( isEmpty()) return;
        programStart( _material );
        programDraw();
    }

    void deleteBuffers() {
        glDeleteBuffers( 1, &vbo );
        glDeleteBuffers( 1, &ibo );
        glDeleteVertexArrays( 1, &vao );
    }

    bool Dynamic() const { return dynamic; }

    void Dynamic( bool val ) { dynamic = val; }

    std::string Name() const { return mName; }

    void Name( std::string val ) { mName = val; }

    void create( std::shared_ptr<cpuVBIB> vbib );

    uint64_t tag() const {
        return mTag;
    }

    void tag( const uint64_t tag ) {
        mTag = tag;
    }

    template<class V>
    static std::shared_ptr<cpuVBIB>
    create_cpuVBIB( std::shared_ptr<VertexStripIBVB<V>> vbib,
                    std::shared_ptr<RenderMaterial> _material,
                    const std::string& _vpName = "unnamed" );

    static uint64_t totalCount() { return sCountInc; }
    static std::string totalCountS() { return std::to_string(sCountInc); }

public:
    static int sNumDrawCalls;
    static int sNumStateChanges;
    static std::set<int64_t> sMatHash;

private:
    void programStart( std::shared_ptr<RenderMaterial> _material ) const;
    void programDraw() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint numIndices = 0;
    Primitive primitveType = PRIMITIVE_TRIANGLE_STRIP;
    std::shared_ptr<RenderMaterial> material;
    uint64_t mTag = GT_Generic;

    bool dynamic = false;
    std::string mName;

    static int64_t  sMaterialHash;
    static uint64_t sCountInc;
};

template<class V>
std::shared_ptr<cpuVBIB>
VertexProcessing::create_cpuVBIB( std::shared_ptr<VertexStripIBVB<V>> vbib,
                                  std::shared_ptr<RenderMaterial> _material,
                                  const std::string& _vpName ) {
    std::shared_ptr<cpuVBIB> ret = std::make_shared<cpuVBIB>();

    ret->elenentSize = sizeof( V );
    ret->bufferVerts = std::make_unique<char[]>( ret->elenentSize * vbib->numVerts );
    std::memcpy( ret->bufferVerts.get(), vbib->verts.get(), ret->elenentSize * vbib->numVerts );
    ret->bufferIndices = std::move( vbib->indices );
    ret->material = _material;
    ret->numIndices = vbib->numIndices;
    ret->numVerts = vbib->numVerts;
    ret->primiteType = vbib->primiteType;
    ret->name = _vpName;
    ret->vElementAttribSize = V::numElements();
    for ( int32_t t = 0; t < V::numElements(); t++ ) {
        ret->vElementAttrib[t].offset = V::offset( t );
        ret->vElementAttrib[t].size = V::size( t );
    }

    sCountInc++;
    return ret;
}
