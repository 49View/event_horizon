
layout( std140 ) uniform LightingUniforms {
    vec3 u_pointLightPos[16];
    vec3 u_pointLightDir[16];
    vec3 u_pointLightIntensity[16];
    vec3 u_pointLightAttenuation[16];
    vec3 u_spotLightBeamDir[16];
    float u_outerCutOff[16];
    int  u_lightType[16];

    vec3 u_sunDirection;
    vec3 u_sunPosition;
    vec4 u_sunRadiance;
    mat4 u_mvpShadowMap;
    mat4 u_depthBiasMVP;
    int  u_numPointLights;
    vec4 u_hdrExposures;
    vec4 u_shadowParameters;
    mat3 shLightCoeffs;
    int  u_hemisphereSampleKernelSize;
    vec3 u_hemisphereSampleKernel[64];
    vec4 u_ssaoParameters;
};
