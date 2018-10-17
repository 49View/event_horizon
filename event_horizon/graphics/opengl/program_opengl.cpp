#include <fstream>

#include "program_opengl.h"

#include "shader_opengl.h"
//#include <unistd.h>

const int32_t ProgramOpenGL::CACHE_FILE_MARK = 0xaabbccdd;

GLuint ProgramOpenGL::build( std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> tesselationControlShader, std::shared_ptr<Shader> tesselationEvaluationShader,
                       std::shared_ptr<Shader> geometryShader, std::shared_ptr<Shader> fragmentShader, std::shared_ptr<Shader> computeShader ) {
    GLuint handle;
    GLCALLRET(handle, glCreateProgram());
    if ( handle > 0 ) {
        //	LOGI("Creating new program: (%d), handle=%d (%s, %s)\n", mId, handle, vertexShader->getId(), fragmentShader->getId());

        // Check if we need to compile the shaders
        if ( vertexShader && !vertexShader->isCompiled()) {
            auto r = vertexShader->compile();
            if ( r == 0 ) return r;
        }
        if ( vertexShader ) GLCALL( glAttachShader( handle, vertexShader->getHandle()));

        if ( tesselationControlShader && !tesselationControlShader->isCompiled()) {
            auto r = tesselationControlShader->compile();
            if ( r == 0 ) return r;
        }
        if ( tesselationControlShader ) GLCALL( glAttachShader( handle, tesselationControlShader->getHandle()));

        if ( tesselationEvaluationShader && !tesselationEvaluationShader->isCompiled()) {
            auto r = tesselationEvaluationShader->compile();
            if ( r == 0 ) return r;
        }
        if ( tesselationEvaluationShader ) GLCALL( glAttachShader( handle, tesselationEvaluationShader->getHandle()));

        if ( geometryShader && !geometryShader->isCompiled()) {
            auto r = geometryShader->compile();
            if ( r == 0 ) return r;
        }
        if ( geometryShader ) GLCALL( glAttachShader( handle, geometryShader->getHandle()));

        if ( fragmentShader && !fragmentShader->isCompiled()) {
            auto r = fragmentShader->compile();
            if ( r == 0 ) return r;
        }
        if ( fragmentShader ) GLCALL( glAttachShader( handle, fragmentShader->getHandle()));

        if ( computeShader && !computeShader->isCompiled()) {
            auto r = computeShader->compile();
            if ( r == 0 ) return r;
        }
        if ( computeShader ) GLCALL( glAttachShader( handle, computeShader->getHandle()));

        GLCALL( glLinkProgram( handle ));

        GLint linkStatus = GL_FALSE;
        GLCALL( glGetProgramiv( handle, GL_LINK_STATUS, &linkStatus ));
        if ( linkStatus != GL_TRUE ) {
            char infoBuffer[1024];
            GLCALL( glGetProgramInfoLog( handle, static_cast<int>( sizeof( infoBuffer )), NULL, infoBuffer ));
            LOGE( "Could not link program %s\n%s\n", mId.c_str(), infoBuffer );
            SHADER_ERROR( "Could not link program", mId.c_str(), infoBuffer );
            handle = 0;
            ASSERT( false );
            return handle;
        }
        int total = -1;
        glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &total );
        for(int i=0; i<total; ++i)  {
            int name_len=-1, num=-1;
            GLenum type = GL_ZERO;
            char name[100];
            glGetActiveUniform( handle, GLuint(i), sizeof(name)-1,
                                &name_len, &num, &type, name );
            name[name_len] = 0;
            GLuint location = glGetUniformLocation( handle, name );
            uniforms[name] = {name, GLToUniformFormat(type), location};
            //LOGI( "uniform %s on location %d with type %d", name, location, type);
        }
    } else {
        LOGI( "Cound not create program: %s", mId.c_str());
        ASSERT( false );
    }
    return handle;
}

bool ProgramOpenGL::createOrUpdate( std::shared_ptr<Shader> vertexShader,
                                    std::shared_ptr<Shader> tesselationControlShader,
                                    std::shared_ptr<Shader> tesselationEvaluationShader,
                                    std::shared_ptr<Shader> geometryShader,
                                    std::shared_ptr<Shader> fragmentShader,
                                    std::shared_ptr<Shader> computeShader, const std::string& cacheFolder,
                                    const std::string& cacheLabel, CreateUpdateFlag cou ) {

    if ( mHandle != 0 ) glDeleteProgram( mHandle );

    mHandle = build( vertexShader, tesselationControlShader, tesselationEvaluationShader, geometryShader,
                     fragmentShader, computeShader );

    // Try to cache it
    cacheProgram( mHandle, cacheFolder, cacheLabel );

    return mHandle != 0;
}

#ifdef CACHE_SHADERS
GLuint ProgramOpenGL::createFromCache( const std::string& cacheFolder, const std::string& cacheLabel, bool simpleVersion ) {
    GLuint handle = 0;
    //	if (isExtensionAvailable("GL_OES_get_program_binary")) {
    if ( cacheFolder.size() > 0 && cacheLabel.size() > 0 ) {
        std::string filename = getFilename( cacheLabel );
        std::string path = cacheFolder + "/" + filename + ( simpleVersion ? "s" : "" );

        std::ifstream fp( path, std::ios::binary );
        if ( fp.is_open() ) {
            GLsizei length = 0;
            GLenum format = 0;
            int32_t mark = 0;

            fp.read( reinterpret_cast<char*>( &mark ), sizeof( mark ) );
            if ( mark == CACHE_FILE_MARK ) {
                fp.read( reinterpret_cast<char*>( &length ), sizeof( length ) );
                fp.read( reinterpret_cast<char*>( &format ), sizeof( format ) );

                uint8_t* data = new uint8_t[length];
                fp.read( reinterpret_cast<char*>( data ), length );

                GLint success = 0;
                GLint activeUniforms = 0;

                // We have a binary ready to go
                GLCALL( handle = glCreateProgram() );
                GLCALL( glProgramBinary( handle, format, data, length ) );

                // Make double sure everything went ok
                GLCALL( glGetProgramiv( handle, GL_LINK_STATUS, &success ) );
                GLCALL( glGetProgramiv( handle, GL_ACTIVE_UNIFORMS, &activeUniforms ) );
                ASSERT( success > 0 );
                ASSERT( activeUniforms > 0 );

                LOGI( "Reloading program binary %s, link status: %d, length: %d bytes, handle=%d", filename.c_str(), success, length, handle );

                // Check that the shader linked
                // HACK: Check also that there are some uniforms in the shader, this is because some drivers are buggy and will report a bogus link status
                if ( !success || activeUniforms == 0 ) {
                    handle = 0;
                    LOGI( "Failed to load precached program" );
                }

                delete[] data;
            }
            fp.close();
        }
    }
    //	}

    return handle;
}
#else

GLuint ProgramOpenGL::createFromCache( const std::string& /*cacheFolder*/, const std::string& /*cacheLabel*/,
                                 bool /*simpleVersion*/ ) { return 0; }

#endif    // Cached Shaders

#ifdef CACHE_SHADERS
void ProgramOpenGL::cacheProgram( GLuint handleToCache, const std::string& cacheFolder, const std::string& cacheLabel, bool simpleVersion ) {
    // Cache this shader if the extension is available
//	if (mHandle != 0 && isExtensionAvailable("GL_OES_get_program_binary")) {
    if ( handleToCache != 0 ) {
        if ( cacheFolder.size() > 0 && cacheLabel.size() > 0 ) {
            std::string filename = getFilename( cacheLabel );
            std::string path = cacheFolder + "/" + filename + ( simpleVersion ? "s" : "" );
            GLsizei length = 0;
            GLenum format = 0;

            // Get the length
            GLCALL( glGetProgramiv( handleToCache, GL_PROGRAM_BINARY_LENGTH, &length ) );

            // Allocate some memory, use the operator new function as we need a void* array and we don't want to have to cast stuff all over the place
            uint8_t* data = new uint8_t[length];

            // Get the binary. The length here sometimes doesn't match the one returned by glGetProgramiv()!, so just discard it
            GLint outLength = 0;
            GLCALL( glGetProgramBinary( handleToCache, length, &outLength, &format, data ) );
            ASSERT( outLength > 0 );

            std::ofstream fp( path, std::ios::binary );
            if ( fp.is_open() ) {
                fp.write( reinterpret_cast<const char*>( &CACHE_FILE_MARK ), sizeof( CACHE_FILE_MARK ) );
                fp.write( reinterpret_cast<char*>( &length ), sizeof( length ) );
                fp.write( reinterpret_cast<char*>( &format ), sizeof( format ) );
                fp.write( reinterpret_cast<char*>( data ), length );
                fp.close();
                LOGI( "Cached program to file: %s, %d bytes", filename.c_str(), length );
            }

            delete[] data;
        }
    }
}
#else

void
ProgramOpenGL::cacheProgram( GLuint /*handleToCache*/, const std::string& /*cacheFolder*/, const std::string& /*cacheLabel*/,
                       bool /*simpleVersion*/ ) {}

#endif

std::string ProgramOpenGL::getFilename( const std::string& cacheLabel ) const {
    //	return "";
    return mId + std::string( "_" ) + cacheLabel + ".bin";
}

