//
// Created by Dado on 12/12/2017.
//

#include "../shader_opengl.h"

GLenum Shader::getShaderType() const {
    switch ( mType ) {
        case Shader::TYPE_VERTEX_SHADER:
            return GL_VERTEX_SHADER;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            return GL_TESS_CONTROL_SHADER;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            return GL_TESS_EVALUATION_SHADER;
        case Shader::TYPE_GEOMETRY_SHADER:
            return GL_GEOMETRY_SHADER;
        case Shader::TYPE_FRAGMENT_SHADER:
            return GL_FRAGMENT_SHADER;
        case Shader::TYPE_COMPUTE_SHADER:
#ifdef OSX
            // OSX doesn't support compute shaders yet, or ever, just assign a random one, it won't be used
            return GL_GEOMETRY_SHADER;
#else
            return GL_COMPUTE_SHADER;
#endif
            break;
        default:
            return GL_VERTEX_SHADER;
    }

}
