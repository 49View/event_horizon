#include <fstream>

#include "program_opengl.h"

#include "shader_opengl.h"
//#include <unistd.h>

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
            char name[100];
            glGetActiveUniform( mHandle, GLuint(i), sizeof(name)-1,
                                &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = static_cast<GLuint>(glGetUniformLocation( mHandle, name ));
            uniforms[name] = {name, GLToUniformFormat(type), location};
            //LOGI( "uniform %s on location %d with type %d", name, location, type);
        }
    } else {
        LOGI( "Cound not create program: %s", mId.c_str());
        ASSERT( false );
    }

    return mHandle != 0;
}
