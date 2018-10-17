//
// Created by Dado on 28/12/2017.
//

#include "shader_manager_opengl.h"

void ShaderManager::addLayoutInjections() {
    shaderInjection["layout_pos2d"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
    )";

    shaderInjection["layout_pos3d"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
    )";

    shaderInjection["layout_pos_tex2d"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       layout( location = 1 ) in vec2 a_t1;

       out vec2 v_texCoord;
    )";

    shaderInjection["layout_pos_tex3d"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       layout( location = 1 ) in vec2 a_t1;

       out vec2 v_texCoord;
    )";

    shaderInjection["layout_font"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       layout( location = 1 ) in vec2 a_t1;
       out vec3 tpos;
    )";

    shaderInjection["layout_pos_texcubemap"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       out vec3 v_texCoord;
    )";

    shaderInjection["layout_sh"] =
    R"(#version #opengl_version
       layout( location = 0 ) in vec3 a_position;
       layout( location = 1 ) in vec2 a_t1;
       layout( location = 2 ) in vec3 a_t2;
       layout( location = 3 ) in vec3 a_t3;
       layout( location = 4 ) in vec3 a_t4;

       layout( location = 5 ) in vec3 shR1;
       layout( location = 6 ) in vec3 shR2;
       layout( location = 7 ) in vec3 shR3;

       layout( location = 8 ) in vec3 shG1;
       layout( location = 9 ) in vec3 shG2;
       layout( location = 10 ) in vec3 shG3;

       layout( location = 11 ) in vec3 shB1;
       layout( location = 12 ) in vec3 shB2;
       layout( location = 13 ) in vec3 shB3;

       out vec2 v_texCoord;
       out vec3 v_color;
       out vec3 v_norm;
       out vec3 v_tan;
       out vec3 v_bitan;
       out vec3 v_shadowmap_coord3;

       out vec3 Position_worldspace;
    )";
}

void ShaderManager::addUniformsInjections() {

    shaderInjection["camera_uniforms"] =
    R"(
        layout( std140 ) uniform CameraUniforms{
            mat4 u_mvpMatrix;
            mat4 u_viewMatrix;
            mat4 u_projMatrix;
            mat4 u_screenSpaceMatrix;
            vec3 u_eyePos;
        };
    )";

    shaderInjection["lighting_uniforms"] =
    R"(
        layout( std140 ) uniform LightingUniforms {
            vec3 u_pointLightPos[16];
            vec3 u_pointLightDir[16];
            vec3 u_pointLightIntensity[16];
            vec3 u_pointLightAttenuation[16];
            vec3 u_spotLightBeamDir[16];
            float u_outerCutOff[16];
            int  u_lightType[16];
            vec3 u_sunDirection;
            mat4 u_mvpShadowMap;
            mat4 u_depthBiasMVP;
            float u_timeOfTheDay;
            int  u_numPointLights;
            mat3 shLightCoeffs;
        };
    )";
}

void ShaderManager::addMacroInjections() {

    shaderInjection["vertex_shader_sh_common"] =
    R"(mat3 shVertexData() {
           vec4 pos4 = vec4( a_position, 1.0 );
           v_texCoord = a_t1;

           //	mat3 modelMatrix3x3 = mat3(transpose(inverse(u_modelMatrix)));//mat3( u_modelMatrix );
           mat3 modelMatrix3x3 = mat3( u_modelMatrix );

           modelMatrix3x3[0] = normalize( modelMatrix3x3[0] );
           modelMatrix3x3[1] = normalize( modelMatrix3x3[1] );
           modelMatrix3x3[2] = normalize( modelMatrix3x3[2] );

           // Position of the vertex, in worldspace : M * position
           Position_worldspace = ( u_modelMatrix * pos4 ).xyz;

           // Shadowmap
           vec4 v_shadowmap_coord = u_depthBiasMVP * vec4( Position_worldspace, 1.0 );
           v_shadowmap_coord.z -= 0.01199;
           v_shadowmap_coord3 = vec3( v_shadowmap_coord.xy, v_shadowmap_coord.z / v_shadowmap_coord.w );

           gl_Position = u_mvpMatrix * u_modelMatrix * pos4;

           v_color = vec3( 0.0 );
           v_color.r += dot( shLightCoeffs[0], shR1 );
           v_color.r += dot( shLightCoeffs[1], shR2 );
           v_color.r += dot( shLightCoeffs[2], shR3 );

           v_color.g += dot( shLightCoeffs[0], shG1 );
           v_color.g += dot( shLightCoeffs[1], shG2 );
           v_color.g += dot( shLightCoeffs[2], shG3 );

           v_color.b += dot( shLightCoeffs[0], shB1 );
           v_color.b += dot( shLightCoeffs[1], shB2 );
           v_color.b += dot( shLightCoeffs[2], shB3 );

           return modelMatrix3x3;
       }
    )";

    shaderInjection["plain_sh_common"] =
    R"(#version #opengl_version

        layout( location = 0 ) out vec4 FragColor;
        layout( location = 1 ) out vec4 BloomColor;

        in vec2 v_texCoord;
        in vec3 v_color;
        in vec3 v_norm;
        in vec3 v_tan;
        in vec3 v_bitan;
        in vec3 v_shadowmap_coord3;

        in vec3 Position_worldspace;

        #include "lighting_uniforms.glsl"
        #include "camera_uniforms.glsl"

        uniform sampler2D diffuseTexture;         // 0 glTextureSlot
        uniform sampler2D normalTexture;          // 1 glTextureSlot
        uniform sampler2D aoTexture;              // 6 glTextureSlot
        uniform sampler2DShadow shadowMapTexture; // 2 glTextureSlot
        uniform sampler2D metallicTexture;        // 8 glTextureSlot
        uniform sampler2D roughnessTexture;
        uniform sampler2D heightTexture;
        // IBL
        uniform samplerCube ibl_irradianceMap;        // 9 glTextureSlot
        uniform samplerCube ibl_specularMap;         // 10 glTextureSlot
        uniform sampler2D   ibl_brdfLUTMap;              // 11 glTextureSlot

        uniform vec3  diffuseColor;
        uniform vec3  ambient;
        uniform vec3  incandescenceColor;
        uniform float incandescenceFactor;
        uniform float opacity;
        uniform float alpha;
        uniform float diffuse;
        uniform float reflectivity;
        uniform vec3 specularColor;
        uniform float cosinePower;
        uniform float metallicV;
        uniform float roughnessV;
        uniform float aoV;

        float metallic;
        float roughness;
        float ao;
        float height_scale = .04;

        const float PI = 3.14159265359;

        vec2 poissonDisk[4] = vec2[](
            vec2( -0.94201624, -0.39906216 ),
            vec2( 0.94558609, -0.76890725 ),
            vec2( -0.094184101, -0.92938870 ),
            vec2( 0.34495938, 0.29387760 )
            );

        vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
        {
            // number of depth layers
            const float minLayers = 8.0;
            const float maxLayers = 32.0;
            float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
            // calculate the size of each layer
            float layerDepth = 1.0 / numLayers;
            // depth of current layer
            float currentLayerDepth = 0.0;
            // the amount to shift the texture coordinates per layer (from vector P)
            vec2 P = viewDir.xy / viewDir.z * height_scale;
            vec2 deltaTexCoords = P / numLayers;

            // get initial values
            vec2  currentTexCoords     = texCoords;
            float currentDepthMapValue = texture(heightTexture, currentTexCoords).r;

            while(currentLayerDepth < currentDepthMapValue)
            {
                // shift texture coordinates along direction of P
                currentTexCoords -= deltaTexCoords;
                // get depthmap value at current texture coordinates
                currentDepthMapValue = texture(heightTexture, currentTexCoords).r;
                // get depth of next layer
                currentLayerDepth += layerDepth;
            }

            // get texture coordinates before collision (reverse operations)
            vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

            // get depth after and before collision for linear interpolation
            float afterDepth  = currentDepthMapValue - currentLayerDepth;
            float beforeDepth = texture(heightTexture, prevTexCoords).r - currentLayerDepth + layerDepth;

            // interpolation of texture coordinates
            float weight = afterDepth / (afterDepth - beforeDepth);
            vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

            return finalTexCoords;
        }

        // ----------------------------------------------------------------------------
        // Easy trick to get tangent-normals to world-space to keep PBR code simplified.
        // Don't worry if you don't get what's going on; you generally want to do normal
        // mapping the usual way for performance anways; I do plan make a note of this
        // technique somewhere later in the normal mapping tutorial.
        vec3 getNormalFromMap( vec2 texCoords )
        {
            vec3 tangentNormal = texture(normalTexture, texCoords).xyz * 2.0 - 1.0;

            vec3 Q1  = dFdx(Position_worldspace);
            vec3 Q2  = dFdy(Position_worldspace);
            vec2 st1 = dFdx(texCoords);
            vec2 st2 = dFdy(texCoords);

            vec3 N   = normalize(v_norm);
            vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
            vec3 B  = -normalize(cross(N, T));
            mat3 TBN = mat3(T, B, N);

            return normalize(TBN * tangentNormal);
        }

        // ------------ BRDF FUNCTIONS -------------

        vec3 fresnelSchlick(float cosTheta, vec3 F0)
        {
            return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
        }
        // ----------------------------------------------------------------------------
        vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
        {
            return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
        }

        float DistributionGGX( vec3 N, vec3 H, float roughness ) {
            float a = roughness*roughness;
            float a2 = a*a;
            float NdotH = max( dot( N, H ), 0.0 );
            float NdotH2 = NdotH*NdotH;

            float nom = a2;
            float denom = ( NdotH2 * ( a2 - 1.0 ) + 1.0 );
            denom = PI * denom * denom;

            return nom / denom;
        }

        float GeometrySchlickGGX( float NdotV, float roughness ) {
            float r = ( roughness + 1.0 );
            float k = ( r*r ) / 8.0;

            float nom = NdotV;
            float denom = NdotV * ( 1.0 - k ) + k;

            return nom / denom;
        }

        float GeometrySmith( vec3 N, vec3 V, vec3 L, float roughness ) {
            float NdotV = max( dot( N, V ), 0.0 );
            float NdotL = max( dot( N, L ), 0.0 );
            float ggx2 = GeometrySchlickGGX( NdotV, roughness );
            float ggx1 = GeometrySchlickGGX( NdotL, roughness );

            return ggx1 * ggx2;
        }

        vec3 rendering_equation( vec3 albedo, vec3 L, vec3 V, vec3 N, vec3 F0, float radiance ) {
            vec3 H = normalize( L + V );
            float NdotL = max( dot( N, L ), 0.0 );

            vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

            float NDF = DistributionGGX( N, H, roughness );
            float G = GeometrySmith( N, V, L, roughness );

            vec3 nominator = NDF * G * F;
            float denominator = 4.0 * max( dot( N, V ), 0.0 ) * NdotL + 0.001;
            vec3 specular = nominator / denominator;

            vec3 kS = F;
            vec3 kD = vec3( 1.0 ) - kS;

            kD *= 1.0 - metallic;

            return ( kD * albedo.xyz / PI + specular ) * radiance * NdotL;
        }

        // ------------ BRDF FUNCTIONS END -------------

        #define_code light_code

            vec3 R = reflect(-V, N);

            vec3 F0 = vec3( 0.04 );
            F0 = mix( F0, albedo.xyz, metallic );

            vec3 LightDirection_tangentspace = normalize( u_sunDirection );

        vec3 Lo = vec3( 0.0 );

        // #ifdef sh_notexture
        //     V = V;
        // #else
        //     V = itbn * V;
        // #endif

        vec3 sunDir = LightDirection_tangentspace;

        // #ifdef sh_notexture
        // vec3 sunDir = LightDirection_tangentspace;
        // #else
        // vec3 sunDir = itbn * LightDirection_tangentspace;
        // #endif

            Lo += rendering_equation( albedo, sunDir, V, N, F0, 5.0 );

        for ( int i = 0; i < u_numPointLights; i++ ) {
            vec3 plmfrag = u_pointLightPos[i] - Position_worldspace;

        #ifdef sh_notexture
            vec3 L = normalize( plmfrag );
            vec3 spotLightBeamDir = u_spotLightBeamDir[i];
        #else
            vec3 L = itbn * normalize( plmfrag );
            vec3 spotLightBeamDir = itbn * u_spotLightBeamDir[i];
        #endif

            // Spotlight
            //float theta = dot( L, spotLightBeamDir );
            //float intensity = 1.0;
            //float outerCutOff = 0.84;
            //if ( theta > outerCutOff ) {
            //	// spotlight
            //	float epsilon = 0.90 - outerCutOff;
            //	intensity = clamp( ( theta - outerCutOff ) / epsilon, 0.0, 1.0 );
            //	// Attenuation
            //	float distance = length( plmfrag );
            //	float attenuation = 1.0 / ( u_pointLightAttenuation[i].x + u_pointLightAttenuation[i].y * distance + u_pointLightAttenuation[i].z * ( distance * distance ) );
            //	diffuseValue += max( dot( N, L ) * intensity * u_pointLightIntensity[i].x * attenuation, 0.0 );
            //}

            // Pointlight
            float distance = length( plmfrag );
            float attenuation = 1.0 / ( u_pointLightAttenuation[i].x + u_pointLightAttenuation[i].y * distance + u_pointLightAttenuation[i].z * ( distance * distance ) );
            float radiance = u_pointLightIntensity[i].x * attenuation;// *pow( visibility, 4 );

            Lo += rendering_equation( albedo, L, V, N, F0, radiance );
        }

        //    Lo *= v_color;

        #end_code

        #define_code shadow_code
        float visibility = 0.0;
        for ( int i = 0; i < 4; i++ ) {
            int index = i;// int( 16.0*random( vec4( gl_FragCoord.xyy, i ) ) ) % 16;
            visibility += texture( shadowMapTexture, vec3( v_shadowmap_coord3.xy + poissonDisk[index] / 4096.0, v_shadowmap_coord3.z ) ) * 0.25;// * u_timeOfTheDay;
        }
        #end_code

        #define_code final_combine

        //vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(ibl_irradianceMap, N).rgb;
        vec3 diffuseV   = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(ibl_specularMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf  = texture(ibl_brdfLUTMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y) * metallic;

        vec3 ambient = (kD * diffuseV + specular) * ao;

        vec3 finalColor = (Lo * visibility) + ambient;
        FragColor = vec4( finalColor, opacity * alpha );

        //	BloomColor = vec4( ( incandescenceColor * incandescenceFactor ) + max(visibility-1.7, 0.0), 1.0 );
        BloomColor = vec4( ( incandescenceColor * incandescenceFactor * finalColor ), 1.0 );

        #end_code
    )";

}

void ShaderManager::addVertexShaderInjections() {

    shaderInjection["vertex_shader_2d_c.vsh"] =
    R"(#include "layout_pos3d.glsl"
       #include "camera_uniforms.glsl"
       uniform mat4 u_modelMatrix;

       void main() {
       gl_Position = u_screenSpaceMatrix * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_2d_font.vsh"] =
    R"(#include "layout_font.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
           tpos = vec3( a_t1.x*0.5, max( a_t1.x - 1.0, 0.0 ), a_t1.y );
           gl_Position = u_screenSpaceMatrix * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_2d_t.vsh"] =
    R"(#include "layout_pos_tex3d.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
       v_texCoord = a_t1;
       gl_Position = u_screenSpaceMatrix * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_3d_c.vsh"] =
    R"(#include "layout_pos_tex3d.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
       gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_3d_font.vsh"] =
    R"(#include "layout_font.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
       tpos = vec3( a_t1.x*0.5, max( a_t1.x - 1.0, 0.0 ), a_t1.y );
       gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position.xyz, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_3d_sh.vsh"] =
    R"(#include "layout_sh.glsl"
       #include "camera_uniforms.glsl"
       #include "lighting_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       #include "vertex_shader_sh_common.glsl"

       void main() {
       mat3 modelMatrix3x3 = shVertexData();

       v_norm = normalize( modelMatrix3x3 * a_t2 );
       v_tan = normalize( modelMatrix3x3 * a_t3 );
       v_bitan = normalize( modelMatrix3x3 * a_t4 );

       // Check handness
       //	if (dot(cross(v_norm, v_tan), v_bitan) < 0.0) {
       //		v_tan *= -1.0;
       //	}
       mat3 itbn;
       itbn[0] = v_tan;
       itbn[1] = v_bitan;
       itbn[2] = v_norm;
       itbn = transpose( itbn );
       }
    )";

    shaderInjection["vertex_shader_3d_sh_notexture.vsh"] =
    R"(#include "layout_sh.glsl"
       #include "camera_uniforms.glsl"
       #include "lighting_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       #include "vertex_shader_sh_common.glsl"

       void main() {
       mat3 modelMatrix3x3 = shVertexData();

       v_norm = normalize( modelMatrix3x3 * a_t2 );
       }
    )";

    shaderInjection["vertex_shader_3d_t.vsh"] =
    R"(#include "layout_pos_tex3d.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
       v_texCoord = a_t1;
       gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_blitcopy.vsh"] =
    R"(#include "layout_pos_tex2d.glsl"

       void main() {
       v_texCoord = a_t1;
       gl_Position = vec4( a_position.x, a_position.y, 0.0, 1.0 );
       }
    )";

    shaderInjection["vertex_shader_brdf.vsh"] =
    R"(#include "layout_pos_tex3d.glsl"

       void main()
       {
       v_texCoord = a_t1;
       gl_Position = vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["skybox.vsh"] =
    R"(#include "layout_pos_texcubemap.glsl"
       #include "camera_uniforms.glsl"

       void main() {
       mat4 vm_zerod = u_viewMatrix;
       vm_zerod[3] = vec4( 0.0, 0.0, 0.0, 1.0 );
       mat4 vm = u_projMatrix * vm_zerod;
       vec4 pos4 = vm * vec4( a_position, 1.0 );
       gl_Position = pos4.xyww;
       v_texCoord = a_position.xzy;
       }
    )";

    shaderInjection["shadowmap.vsh"] =
    R"(#include "layout_pos3d.glsl"
       #include "lighting_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
           gl_Position = u_mvpShadowMap * u_modelMatrix * vec4( a_position, 1.0 );
       }
    )";

    shaderInjection["plain_cubemap.vsh"] =
    R"(#include "layout_pos_texcubemap.glsl"
       #include "camera_uniforms.glsl"

       void main() {
           mat4 vm = u_projMatrix * u_viewMatrix;
           gl_Position = vm * vec4( a_position, 1.0 );
           v_texCoord = a_position.xyz;
       }
    )";
}

void ShaderManager::addFragmentShaderInjections() {
    shaderInjection["skybox.fsh"] =
    R"(#version #opengl_version
       in vec3 v_texCoord;
       out vec4 color;

       //#include "lighting_uniforms.glsl"

       //uniform samplerCube cubeMapTexture;

       void main()
       {

           vec3 light_pos = normalize( -u_sunDirection );

           // Base is gradient between horizon and zenith color based on altitude
           vec4 ambient = vec4(0.3, 0.5, 0.7, 1.0);
           vec4 diffuse = vec4(0.1, 0.5, 0.66, 1.0);
           color = mix(ambient, diffuse, v_texCoord.y);

           // Sample texture and blend into color
       //	vec4 t2 = texture(u_tex0, v_tex_coord);
       //	color = mix(color, t2, u_material.custom.g);

           // Include corona color based on light position; dot-value is multiplied by small
           // constant to achieve disc effect at the center of the corona
           float s = clamp(1.015 * dot(v_texCoord, light_pos), 0.0, 1.0);
           vec4 specular = vec4(10.0);
           float r = 5.0; //u_material.custom.r
           color = mix(color, specular, pow(s, r));

           //color = vec4( pow( texture( cubeMapTexture, v_texCoord ).xyz, vec3( 2.2 ) ), 1.0 );

           // TODO: dither color to avoid banding
           //	http://www.anisopteragames.com/how-to-fix-color-banding-with-dithering/

       //    color = vec4( v_texCoord.xyz * 0.5 + 0.5, 1.0);
       //    color = vec4( v_texCoord.xyz * 0.5f + 0.5f, 1.0);

       //    color = vec4( 1.0 );
       //    if ( v_texCoord.x < 0.0 ) {
       //        color = vec4( 1.0, 0.0, 1.0, 1.0);
       //    }
       }
    )";

    shaderInjection["shadowmap.fsh"] =
    R"(#version #opengl_version

       layout(location = 0) out float FragDepth;
       //layout(location = 0) out vec4 FragDepth;

       void main() {
           FragDepth = gl_FragCoord.z;
           //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
           //	FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
       }
    )";

    shaderInjection["plain_font.fsh"] =
    R"(#version #opengl_version
       uniform vec3   diffuseColor;
       uniform float  alpha;
       uniform float  opacity;

       in vec3 tpos;
       out vec4 FragColor;

       void main()
       {
           float alphaV = 1.0;
           if (tpos.z != 0.0)
           {
               vec2 p = tpos.xy;
               // Gradients
               vec2 px = dFdx(p);
               vec2 py = dFdy(p);
               // Chain rule
               float fx = ((2.0*p.x)*px.x - px.y);
               float fy = ((2.0*p.x)*py.x - py.y);
               // Signed distance
               float dist = fx*fx + fy*fy;
               float sd = (p.x*p.x - p.y)*-tpos.z/sqrt(dist);
               // Linear alpha
               alphaV = clamp(0.5 - sd, 0.0, 1.0);
           }
           FragColor = vec4(diffuseColor, alpha*alphaV*opacity);
       }
    )";

    shaderInjection["plain_blur_horizontal.fsh"] =
    R"(#version #opengl_version
       out vec4 FragColor;
       in vec2 v_texCoord;

       uniform sampler2D colorFBTexture;
       float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

       void main()
       {
           vec2 tex_offset = vec2(1.0) / vec2(textureSize(colorFBTexture, 0)); // gets size of single texel
           vec3 result = texture(colorFBTexture, v_texCoord).rgb * weight[0]; // current fragment's contribution

           for(int i = 1; i < 5; ++i)
           {
               result += texture(colorFBTexture, v_texCoord + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
               result += texture(colorFBTexture, v_texCoord - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
           }

           FragColor = vec4(result, 0.0);
       }
    )";

    shaderInjection["plain_blur_vertical.fsh"] =
    R"(#version #opengl_version
       out vec4 FragColor;
       in vec2 v_texCoord;

       uniform sampler2D colorFBTexture;
       float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

       void main()
       {
           vec2 tex_offset = vec2(1.0) / vec2(textureSize(colorFBTexture, 0)); // gets size of single texel
           vec4 result = texture(colorFBTexture, v_texCoord) * weight[0]; // current fragment's contribution

           for(int i = 1; i < 5; ++i)
           {
               result += texture(colorFBTexture, v_texCoord + vec2(0.0, tex_offset.y * float(i))) * weight[i];
               result += texture(colorFBTexture, v_texCoord - vec2(0.0, tex_offset.y * float(i))) * weight[i];
           }

           FragColor = vec4( result.xyz, 0.0 );
       }
    )";

    shaderInjection["plain_final_combine.fsh"] =
    R"(#version #opengl_version

       in vec2 v_texCoord;
       out vec4 FragColor;
       uniform sampler2D colorFBTexture;
       //uniform sampler2D bloomTexture;
       uniform sampler2D shadowMapTexture;

       void main()
       {
           vec4 sceneColor = texture(colorFBTexture, v_texCoord);
       //    vec4 bloomColor = texture(bloomTexture, v_texCoord);

       float shadowColor = texture(shadowMapTexture, v_texCoord).x;
       vec3 shadowMask = vec3(shadowColor, shadowColor,shadowColor);
       FragColor = vec4( shadowMask, 1.0 );
       //  FragColor = vec4( mix(shadowMask, ( sceneColor.xyz + bloomColor.xyz ), vec3(0.5)), 1.0);

           //FragColor = sceneColor + bloomColor;
           //FragColor.xyz /= ( FragColor.xyz + vec3( 1.0 ) );
           //FragColor = vec4( pow( FragColor.xyz, vec3( 1.0 / 2.2 ) ), FragColor.a );
           FragColor = sceneColor;
       }
    )";

    shaderInjection["fragment_shader_color.fsh"] =
    R"(#version #opengl_version

       uniform float alpha;
       uniform float opacity;
       uniform vec3  diffuseColor;

       out vec4 FragColor;

       void main()
       {
           FragColor = vec4(diffuseColor, alpha*opacity);
       }
    )";

    shaderInjection["fragment_shader_texture.fsh"] =
    R"(#version #opengl_version

       in vec2 v_texCoord;
       out vec4 FragColor;

       uniform sampler2D colorTexture;
       uniform float alpha;
       uniform float opacity;
       uniform vec3 diffuseColor;

       void main()
       {
           vec4 textureCol = texture(colorTexture, v_texCoord);
           FragColor = vec4( textureCol.rgb * diffuseColor, min(alpha, textureCol.a*opacity));
       }
    )";

    shaderInjection["plain_cubemap.fsh"] =
    R"(#version #opengl_version
       in vec3 v_texCoord;
       out vec4 color;

       uniform samplerCube cubeMapTexture;

       void main()
       {
           color = vec4( texture(cubeMapTexture, v_texCoord).xyz, 1.0);
       }
    )";

    shaderInjection["irradiance_convolution.fsh"] =
    R"(#version #opengl_version
       out vec4 FragColor;
       in vec3 v_texCoord;

       uniform samplerCube cubeMapTexture;

       const float PI = 3.14159265359;

       void main()
       {
           // The world vector acts as the normal of a tangent surface
           // from the origin, aligned to WorldPos. Given this normal, calculate all
           // incoming radiance of the environment. The result of this radiance
           // is the radiance of light coming from -Normal direction, which is what
           // we use in the PBR shader to sample irradiance.
           vec3 N = normalize(v_texCoord);

           vec3 irradiance = vec3(0.0);

           // tangent space calculation from origin point
           vec3 up    = vec3(0.0, 1.0, 0.0);
           vec3 right = cross(up, N);
           up            = cross(N, right);

           float sampleDelta = 0.025;
           float nrSamples = 0.0;
           for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
           {
               for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
               {
                   // spherical to cartesian (in tangent space)
                   vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
                   // tangent space to world
                   vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

                   irradiance += texture(cubeMapTexture, sampleVec).rgb * cos(theta) * sin(theta);
                   nrSamples++;
               }
           }
           irradiance = PI * irradiance * (1.0 / float(nrSamples));

           FragColor = vec4(irradiance, 1.0);
       //    FragColor = vec4( texture(cubeMapTexture, v_texCoord).xyz, 1.0);
       }
    )";

    shaderInjection["ibl_specular_prefilter.fsh"] =
    R"(#version #opengl_version
       out vec4 FragColor;
       in vec3 v_texCoord;

       uniform samplerCube cubeMapTexture;
       uniform float roughness;

       const float PI = 3.14159265359;
       // ----------------------------------------------------------------------------
       float DistributionGGX(vec3 N, vec3 H, float roughness)
       {
           float a = roughness*roughness;
           float a2 = a*a;
           float NdotH = max(dot(N, H), 0.0);
           float NdotH2 = NdotH*NdotH;

           float nom   = a2;
           float denom = (NdotH2 * (a2 - 1.0) + 1.0);
           denom = PI * denom * denom;

           return nom / denom;
       }
       // ----------------------------------------------------------------------------
       // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
       // efficient VanDerCorpus calculation.
       float RadicalInverse_VdC(uint bits)
       {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
       }
       // ----------------------------------------------------------------------------
       vec2 Hammersley(uint i, uint N)
       {
           return vec2(float(i)/float(N), RadicalInverse_VdC(i));
       }
       // ----------------------------------------------------------------------------
       vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
       {
           float a = roughness*roughness;

           float phi = 2.0 * PI * Xi.x;
           float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
           float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

           // from spherical coordinates to cartesian coordinates - halfway vector
           vec3 H;
           H.x = cos(phi) * sinTheta;
           H.y = sin(phi) * sinTheta;
           H.z = cosTheta;

           // from tangent-space H vector to world-space sample vector
           vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
           vec3 tangent   = normalize(cross(up, N));
           vec3 bitangent = cross(N, tangent);

           vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
           return normalize(sampleVec);
       }
       // ----------------------------------------------------------------------------
       void main()
       {
           vec3 N = normalize(v_texCoord);

           // make the simplyfying assumption that V equals R equals the normal
           vec3 R = N;
           vec3 V = R;

           const uint SAMPLE_COUNT = 1024u;
           vec3 prefilteredColor = vec3(0.0);
           float totalWeight = 0.0;

           for(uint i = 0u; i < SAMPLE_COUNT; ++i)
           {
               // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
               vec2 Xi = Hammersley(i, SAMPLE_COUNT);
               vec3 H = ImportanceSampleGGX(Xi, N, roughness);
               vec3 L  = normalize(2.0 * dot(V, H) * H - V);

               float NdotL = max(dot(N, L), 0.0);
               if(NdotL > 0.0)
               {
                   // sample from the environment's mip level based on roughness/pdf
                   float D   = DistributionGGX(N, H, roughness);
                   float NdotH = max(dot(N, H), 0.0);
                   float HdotV = max(dot(H, V), 0.0);
                   float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

                   float resolution = 512.0; // resolution of source cubemap (per face)
                   float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
                   float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

                   float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

                   prefilteredColor += textureLod(cubeMapTexture, L, mipLevel).rgb * NdotL;
                   totalWeight      += NdotL;
               }
           }

           prefilteredColor = prefilteredColor / totalWeight;

           FragColor = vec4(prefilteredColor, 1.0);
       }
    )";

    shaderInjection["plain_brdf.fsh"] =
    R"(#version #opengl_version
       out vec2 FragColor;
       in vec2 v_texCoord;

       const float PI = 3.14159265359;
       // ----------------------------------------------------------------------------
       // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
       // efficient VanDerCorpus calculation.
       float RadicalInverse_VdC(uint bits)
       {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
       }
       // ----------------------------------------------------------------------------
       vec2 Hammersley(uint i, uint N)
       {
           return vec2(float(i)/float(N), RadicalInverse_VdC(i));
       }
       // ----------------------------------------------------------------------------
       vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
       {
           float a = roughness*roughness;

           float phi = 2.0 * PI * Xi.x;
           float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
           float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

           // from spherical coordinates to cartesian coordinates - halfway vector
           vec3 H;
           H.x = cos(phi) * sinTheta;
           H.y = sin(phi) * sinTheta;
           H.z = cosTheta;

           // from tangent-space H vector to world-space sample vector
           vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
           vec3 tangent   = normalize(cross(up, N));
           vec3 bitangent = cross(N, tangent);

           vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
           return normalize(sampleVec);
       }
       // ----------------------------------------------------------------------------
       float GeometrySchlickGGX(float NdotV, float roughness)
       {
           // note that we use a different k for IBL
           float a = roughness;
           float k = (a * a) / 2.0;

           float nom   = NdotV;
           float denom = NdotV * (1.0 - k) + k;

           return nom / denom;
       }
       // ----------------------------------------------------------------------------
       float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
       {
           float NdotV = max(dot(N, V), 0.0);
           float NdotL = max(dot(N, L), 0.0);
           float ggx2 = GeometrySchlickGGX(NdotV, roughness);
           float ggx1 = GeometrySchlickGGX(NdotL, roughness);

           return ggx1 * ggx2;
       }
       // ----------------------------------------------------------------------------
       vec2 IntegrateBRDF(float NdotV, float roughness)
       {
           vec3 V;
           V.x = sqrt(1.0 - NdotV*NdotV);
           V.y = 0.0;
           V.z = NdotV;

           float A = 0.0;
           float B = 0.0;

           vec3 N = vec3(0.0, 0.0, 1.0);

           const uint SAMPLE_COUNT = 1024u;
           for(uint i = 0u; i < SAMPLE_COUNT; ++i)
           {
               // generates a sample vector that's biased towards the
               // preferred alignment direction (importance sampling).
               vec2 Xi = Hammersley(i, SAMPLE_COUNT);
               vec3 H = ImportanceSampleGGX(Xi, N, roughness);
               vec3 L = normalize(2.0 * dot(V, H) * H - V);

               float NdotL = max(L.z, 0.0);
               float NdotH = max(H.z, 0.0);
               float VdotH = max(dot(V, H), 0.0);

               if(NdotL > 0.0)
               {
                   float G = GeometrySmith(N, V, L, roughness);
                   float G_Vis = (G * VdotH) / (NdotH * NdotV);
                   float Fc = pow(1.0 - VdotH, 5.0);

                   A += (1.0 - Fc) * G_Vis;
                   B += Fc * G_Vis;
               }
           }
           A /= float(SAMPLE_COUNT);
           B /= float(SAMPLE_COUNT);
           return vec2(A, B);
       }
       // ----------------------------------------------------------------------------
       void main()
       {
           vec2 integratedBRDF = IntegrateBRDF(v_texCoord.x, v_texCoord.y);
           FragColor = integratedBRDF;
       }
    )";

    shaderInjection["plain_sh.fsh"] =
    R"(#include "plain_sh_common.glsl"
       #include "texture_notile.glsl"

       void main() {
           mat3 itbn;
           itbn[0] = normalize( v_tan );
           itbn[1] = normalize( v_bitan );
           itbn[2] = normalize( v_norm );
           itbn = transpose( itbn );

           vec3 V = normalize( u_eyePos - Position_worldspace );

           vec3 te = itbn * u_eyePos;
           vec3 tw = itbn * Position_worldspace;
           vec2 texCoords = v_texCoord;//ParallaxMapping(v_texCoord, V);

           ao = texture( aoTexture, texCoords ).x * aoV;
           roughness = texture( roughnessTexture, texCoords ).x * roughnessV;
           metallic = texture( metallicTexture, texCoords ).x * metallicV;
           vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor;

           //vec4 normalCol = texture( normalTexture, texCoords );
           //vec3 N = normalize( normalCol.rgb*2.0 - 1.0 );
           vec3 N = getNormalFromMap(texCoords);

           shadow_code

           light_code

           final_combine
       }
    )";

    shaderInjection["plain_sh_notexture.fsh"] =
    R"(#include "plain_sh_common.glsl"

       #define sh_notexture

       void main() {
           ao = aoV;
           roughness = roughnessV;
           metallic = metallicV;
           vec3 albedo = diffuseColor;
           vec3 N = normalize( v_norm );
           vec3 V = normalize( u_eyePos - Position_worldspace );

           shadow_code

           light_code

           final_combine
       }
    )";
}

void ShaderManager::createInjections() {
    addUniformsInjections();
    addLayoutInjections();
    addMacroInjections();
    addVertexShaderInjections();
    addFragmentShaderInjections();

    createCCInjectionMap();

//    for ( const auto& [k,v] : shaderInjection ) {
//        std::string fkey = ( k.find_last_of('.') == std::string::npos ) ? k + ".glsl" : k;
//        FM::writeLocalTextFile( "ss/" + fkey, v );
//    }
}

void ShaderManager::createInjection( const std::string& key, const std::string& text ) {
    shaderInjection[key] = text;
}
