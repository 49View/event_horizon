
uniform vec3 u_pointLightPos[16];
uniform vec3 u_pointLightDir[16];
uniform vec3 u_pointLightIntensity[16];
uniform vec3 u_pointLightAttenuation[16];
uniform vec3 u_spotLightBeamDir[16];
uniform float u_outerCutOff[16];
// ###WEBGL1###
//uniform int  u_lightType[16];
uniform vec3 u_sunDirection;
uniform vec3 u_sunPosition;
uniform vec4 u_sunRadiance;
uniform mat4 u_mvpShadowMap;
uniform mat4 u_depthBiasMVP;
// ###WEBGL1###
// we can't use non-const variables in foor loop
//uniform int  u_numPointLights;
uniform vec4 u_hdrExposures;
uniform vec4 u_shadowParameters;
uniform mat3 shLightCoeffs;
// ###WEBGL1###
// unused variables are a no-no, remember?
//uniform int  u_hemisphereSampleKernelSize;
uniform vec3 u_hemisphereSampleKernel[64];
uniform vec4 u_ssaoParameters;
