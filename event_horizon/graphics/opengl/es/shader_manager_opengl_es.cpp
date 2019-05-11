#include "../shader_manager_opengl.h"

void ShaderManager::createCCInjectionMap() {
    ccShaderMap["#opengl_version"] = "300 es\nprecision mediump float;\nprecision mediump int;\nprecision mediump sampler2DShadow;";
}

// this was required for font rendering but probably legacy 1.00 GL_ES
// #extension GL_OES_standard_derivatives : enable
