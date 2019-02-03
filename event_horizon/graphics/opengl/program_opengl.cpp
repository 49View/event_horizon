#include <fstream>

#include "program_opengl.h"
#include "shader_opengl.h"
#include <core/heterogeneous_map.hpp>

//#include <unistd.h>

void ProgramOpenGL::setDefaultUniforms( const std::string& _name, GLenum uf ) {

    switch ( uf ) {
        case GL_FLOAT:
            uniformDefaults->assign( _name, 1.0f );
            break;
        case GL_BOOL:
            uniformDefaults->assign( _name, false );
            break;
        case GL_INT:
            uniformDefaults->assign( _name, 0 );
            break;
        case GL_FLOAT_VEC2:
            uniformDefaults->assign( _name, V2f{1.0f} );
            break;
        case GL_FLOAT_VEC3:
            uniformDefaults->assign( _name, V3f{1.0f} );
            break;
        case GL_FLOAT_VEC4:
            uniformDefaults->assign( _name, V4f{1.0f} );
            break;
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
            ASSERTV(0, "Implement VecI(nterger) mapping from shaders");
            break;
        case GL_FLOAT_MAT2:
            ASSERTV(0, "Implement Mat2 mapping from shaders");
            break;
        case GL_FLOAT_MAT3:
            uniformDefaults->assign( _name, Matrix3f::IDENTITY );
            break;
        case GL_FLOAT_MAT4:
            uniformDefaults->assign( _name, Matrix4f::IDENTITY );
            break;
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_2D_SHADOW:
            uniformDefaults->assign( _name, TextureUniformDesc{_name, 0, 0, 0} );
            break;
        default:
            ASSERTV(0, "Unknown uniform mapping %d", uf );
            break;
    };
}

bool ProgramOpenGL::createOrUpdate( std::shared_ptr<Shader> vertexShader,
                                    std::shared_ptr<Shader> tesselationControlShader,
                                    std::shared_ptr<Shader> tesselationEvaluationShader,
                                    std::shared_ptr<Shader> geometryShader,
                                    std::shared_ptr<Shader> fragmentShader,
                                    std::shared_ptr<Shader> computeShader ) {

    if ( mHandle > 0 ) {
        const static size_t maxShaderAttached = 16;
        GLsizei numAttachedShaders = 0;
        GLuint attachedShadersHandles[maxShaderAttached];
        GLCALL( glGetAttachedShaders( mHandle, maxShaderAttached, &numAttachedShaders, attachedShadersHandles ) );
        for ( int t = 0; t < numAttachedShaders; t++ ) {
            GLCALL( glDetachShader(mHandle, attachedShadersHandles[t] ) );
        }
        GLCALL( glDeleteProgram( mHandle ) );
    }
    GLCALLRET(mHandle, glCreateProgram() );
    if ( mHandle > 0 ) {
        uniformDefaults = std::make_shared<Material>(mId, mId);
        //	LOGI("Creating new program: (%d), handle=%d (%s, %s)\n", mId, handle, vertexShader->getId(), fragmentShader->getId());

        // Check if we need to compile the shaders
        if ( vertexShader ) GLCALL( glAttachShader( mHandle, vertexShader->getHandle()));
        if ( tesselationControlShader ) GLCALL( glAttachShader( mHandle, tesselationControlShader->getHandle()));
        if ( tesselationEvaluationShader ) GLCALL( glAttachShader( mHandle, tesselationEvaluationShader->getHandle()));
        if ( geometryShader ) GLCALL( glAttachShader( mHandle, geometryShader->getHandle()));
        if ( fragmentShader ) GLCALL( glAttachShader( mHandle, fragmentShader->getHandle()));
        if ( computeShader ) GLCALL( glAttachShader( mHandle, computeShader->getHandle()));

        GLCALL( glLinkProgram( mHandle ));

        GLint linkStatus = GL_FALSE;
        GLCALL( glGetProgramiv( mHandle, GL_LINK_STATUS, &linkStatus ));
        if ( linkStatus != GL_TRUE ) {
            char infoBuffer[1024];
            GLCALL( glGetProgramInfoLog( mHandle, static_cast<int>( sizeof( infoBuffer )), NULL, infoBuffer ));
            LOGE( "Could not link program %s\n%s\n", mId.c_str(), infoBuffer );
            SHADER_ERROR( "Could not link program", mId.c_str(), infoBuffer );
            mHandle = 0;
            ASSERT( false );
            return false;
        }
        int total = -1;
        glGetProgramiv( mHandle, GL_ACTIVE_UNIFORMS, &total );
        for(int i=0; i<total; ++i)  {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[400];
            glGetActiveUniform( mHandle, GLuint(i), 399, &name_len, &num, &type, name );
            name[name_len] = 0;
            auto location = static_cast<GLint>(glGetUniformLocation( mHandle, name ));
            auto utype = GLToUniformFormat(type);
            uniforms[name] = {name, utype, location};
            if ( location >= 0 && !( name[0] == 'u' && name[1] == '_')) {
                setDefaultUniforms( name, type);
                LOGI( "Shader %s uniform %s on location %d with type %d", mId.c_str(), name, location, type);
            }
        }
    } else {
        LOGI( "Cound not create program: %s", mId.c_str());
        ASSERT( false );
    }

    return mHandle != 0;
}
