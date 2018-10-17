
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
            mat4 u_mvpShadowMap;
            mat4 u_depthBiasMVP;
            float u_timeOfTheDay;
            int  u_numPointLights;
            mat3 shLightCoeffs;
        };
    