//
// Created by Dado on 12/12/2017.
//

#include "../shader_opengl.h"

GLenum Shader::getShaderType() const {
    switch ( mType ) {
        case Shader::TYPE_VERTEX_SHADER:
            return GL_VERTEX_SHADER;
        case Shader::TYPE_FRAGMENT_SHADER:
            return GL_FRAGMENT_SHADER;

        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            return GL_VERTEX_SHADER;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            return GL_VERTEX_SHADER;
        case Shader::TYPE_GEOMETRY_SHADER:
            return GL_VERTEX_SHADER;
        case Shader::TYPE_COMPUTE_SHADER:
            return GL_VERTEX_SHADER;
        default:
            return GL_VERTEX_SHADER;
    }

}
