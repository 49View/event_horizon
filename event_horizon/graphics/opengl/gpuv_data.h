#pragma once

#include <memory>
#include <graphics/opengl/gl_util.h>
#include <graphics/program.h>
#include <graphics/vertex_processing.h>

class RenderMaterial;
struct cpuVBIB;

class GPUVData {
public:
    explicit GPUVData( cpuVBIB&& _vbib );
    void draw( ) const;
    void programStart( RenderMaterial* _material, Program* _program = nullptr ) const;
    void programStart( Program* _program = nullptr ) const;

    void updateVBO( cpuVBIB&& _vbib );
    void updateUVs( const uint32_t *indices, const std::vector<V3f>& _pos, const std::vector<V2f>& _uvs, uint64_t _index, uint64_t _xrefStart );
    bool Dynamic() const { return dynamic; }
    void Dynamic( bool val ) { dynamic = val; }

    void deleteBuffers();

    bool isEmpty() const;

private:
    void setupVIO();
private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint numIndices = 0;
    GLenum primitveType = PRIMITIVE_TRIANGLE_STRIP;
    bool dynamic = false;
    cpuVBIB vbib; // Source data copy, for now we need it to easy remapping
};

