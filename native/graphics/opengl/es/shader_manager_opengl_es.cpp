#include "../shader_manager_opengl.h"

void ShaderManager::createCCInjectionMap() {
    ccShaderMap["#opengl_version"] = "300 es\nprecision mediump float;\nprecision mediump int;\nprecision mediump sampler2DShadow;\nprecision mediump sampler3D;";
#ifdef ANDROID
    ccShaderMap["#precision_high"] = "precision highp float;";
#else
    ccShaderMap["#precision_high"] = "precision mediump float;";
#endif
}

// this was required for font rendering but probably legacy 1.00 GL_ES
// #extension GL_OES_standard_derivatives : enable
