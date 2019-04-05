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
    void programStart( std::shared_ptr<RenderMaterial> _material ) const;
    void create( std::shared_ptr<cpuVBIB> vbib );

    bool Dynamic() const { return dynamic; }
    void Dynamic( bool val ) { dynamic = val; }

    void deleteBuffers() {
        GLCALL(glDeleteBuffers( 1, &vbo ));
        GLCALL(glDeleteBuffers( 1, &ibo ));
        GLCALL(glDeleteVertexArrays( 1, &vao ));
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

class RenderChunk {
public:

    void create( std::shared_ptr<cpuVBIB> vbib );

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
};
