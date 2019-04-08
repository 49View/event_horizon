#pragma once

#include <memory>
#include <graphics/opengl/gl_util.h>

class RenderMaterial;
struct cpuVBIB;

class GPUVData {
public:
    explicit GPUVData( const cpuVBIB& _vbib );
    void draw() const;
    void programStart( std::shared_ptr<RenderMaterial> _material ) const;

    bool Dynamic() const { return dynamic; }
    void Dynamic( bool val ) { dynamic = val; }

    void deleteBuffers();

    bool isEmpty() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint numIndices = 0;
    GLenum primitveType = PRIMITIVE_TRIANGLE_STRIP;
    bool dynamic = false;
};

