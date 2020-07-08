#pragma once

#ifdef OSX
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <GLFW/glfw3.h>
#elif ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif defined(WIN32)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#elif defined(LINUX)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#ifdef _WEBGL1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GLES3/gl3.h>
#endif

#endif