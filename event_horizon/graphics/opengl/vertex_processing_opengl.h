#pragma once

#include <memory>
#include <graphics/opengl/gl_util.h>

class RenderMaterial;
struct cpuVBIB;

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

class GPUVData {
public:
    void draw() const;
    void programStart( std::shared_ptr<RenderMaterial> _material ) const;
    void create( std::shared_ptr<cpuVBIB> vbib );

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

