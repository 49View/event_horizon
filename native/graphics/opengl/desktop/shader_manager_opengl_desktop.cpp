#include "../shader_manager_opengl.h"

void ShaderManager::createCCInjectionMap() {
    ccShaderMap["#opengl_version"] = "410";
    ccShaderMap["#precision_high"] = "precision highp float;";
}
