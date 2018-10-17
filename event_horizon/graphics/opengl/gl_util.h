#pragma once

#include <cstring>
#include "core/util.h"
#include "gl_headers.hpp"
#include "../graphic_constants.h"
#include "core/soa_utils.h"

void setTextureAligment( int i );
void checkGlError( const char* op, int line, const char* file );
void shaderError( const char* error, const char* title, const char* details );
void shaderError( const char* error, int32_t title, const char* details );
void glPerfTimerStart();
void glPerfTimerEnd( const char* name );
const char* glFormatToString( int format );
const char* glPixelTypeToString( int pixelType );
const char* glRenderBufferFormatToString( int renderBufferFormat );
void printGLString( const char *name, GLenum s );
bool isExtensionAvailable( const char* extensionName );
int getMaxTextureSize();
int getMaxRenderbufferSize();
bool get24BitRenderbuffersSupported();

GLenum pixelFormatToGlInternalFormat( PixelFormat pixelFormat );
GLenum pixelFormatToGlInternalFormatSized( PixelFormat pixelFormat );
GLenum pixelFormatToGlFormat( PixelFormat pixelFormat );
GLenum pixelFormatToGlType( PixelFormat pixelFormat );
GLenum primitiveToGl( Primitive primitive );
GLenum filterToGl( Filter filter );
GLenum wrapModeToGl( WrapMode mode );
GLenum cullModeToGl( CullMode mode );
GLenum depthFunctionToGl( DepthFunction mode );
GLenum targetToGl( TextureTargetMode mode );
GLenum imageTargetToGl( TextureTargetMode mode, bool multiSampled );
GLenum nameToCubeMapSide( const std::string& name);

UniformFormat GLToUniformFormat( GLenum uf );

const char* pixelFormatToString( PixelFormat pixelFormat );
const char* wrapModeToString( WrapMode wrapMode );
int getPixelFormatChannels( PixelFormat pixelFormat );

//#define _ULTRADEBUG

#ifdef _ULTRADEBUG
#define SHADER_ERROR(X, Y, Z) /*shaderError(X, Y, Z)*/
#define GL_PERF_TIMER_START() glPerfTimerStart()
#define GL_PERF_TIMER_END(NAME) glPerfTimerEnd(NAME)
#define GLCALL(X) do { X; checkGlError(#X, __LINE__, __FILE__); } while (0)
#define GLCALLRET(R, X) do { R = X; checkGlError(#X, __LINE__, __FILE__); } while(0)
#else
#define SHADER_ERROR(X, Y, Z)
#define GL_PERF_TIMER_START()
#define GL_PERF_TIMER_END(NAME)
#define GLCALL(X) X
#define GLCALLRET(R, X) do { R = X; } while(0)
#endif

