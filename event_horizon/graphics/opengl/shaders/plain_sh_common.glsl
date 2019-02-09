#version #opengl_version

layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 BloomColor;

in vec2 v_texCoord;
in vec2 v_texCoord2;
in vec3 v_color;
in vec3 v_norm;
in vec3 v_tan;
in vec3 v_bitan;
in vec3 v_shadowmap_coord3;
in vec3 Position_worldspace;
in vec4 v_t8;

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"
#include "color_uniforms.glsl"

uniform sampler2D diffuseTexture;         // 0 glTextureSlot
uniform sampler2D normalTexture;          // 1 glTextureSlot
uniform sampler2DShadow shadowMapTexture; // 2 glTextureSlot
uniform sampler2D aoTexture;              // 6 glTextureSlot
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;        // 8 glTextureSlot
uniform sampler2D heightTexture;
uniform sampler2D lightmapTexture;
// IBL
uniform samplerCube ibl_irradianceMap;        // 9 glTextureSlot
uniform samplerCube ibl_specularMap;         // 10 glTextureSlot
uniform sampler2D   ibl_brdfLUTMap;              // 11 glTextureSlot

uniform vec3  ambient;
uniform vec3  incandescenceColor;
uniform float incandescenceFactor;
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
    // return v_norm;
    // vec3 tangentNormal = texture(normalTexture, texCoords).xyz * 2.0 - 1.0;

    // vec3 Q1  = dFdx(Position_worldspace);
    // vec3 Q2  = dFdy(Position_worldspace);
    // vec2 st1 = dFdx(texCoords);
    // vec2 st2 = dFdy(texCoords);

    // vec3 N  = normalize(v_norm);
    // vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    // vec3 B  = normalize(cross(N, T));

    // mat3 TBN = mat3(T, B, N);

    // return normalize(TBN * tangentNormal);

    vec3 tangentNormal = texture(normalTexture, texCoords).xyz * 2.0 - 1.0;
    mat3 TBN = mat3(v_tan, v_bitan, v_norm);
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
    float denominator = (4.0 * max( dot( N, V ), 0.0 ) * NdotL) + 0.000001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3( 1.0 ) - kS;

    kD *= 1.0 - metallic;

    return ( (kD * (albedo.xyz / PI)) + specular ) * radiance * NdotL;
}

// ------------ BRDF FUNCTIONS END -------------

#define_code light_code

    vec3 F0 = vec3( 0.04 );
    F0 = mix( F0, albedo.xyz, metallic );

    vec3 Lo = vec3( 0.0 );

    vec3 L_Sun = normalize( u_sunPosition - Position_worldspace );
    Lo += rendering_equation( albedo, L_Sun, V, N, F0, 30.0 );

// for ( int i = 0; i < u_numPointLights; i++ ) {
//     vec3 plmfrag = u_pointLightPos[i] - Position_worldspace;

// #ifdef sh_notexture
//     vec3 L = normalize( plmfrag );
//     vec3 spotLightBeamDir = u_spotLightBeamDir[i];
// #else
//     vec3 L = itbn * normalize( plmfrag );
//     vec3 spotLightBeamDir = itbn * u_spotLightBeamDir[i];
// #endif

//     // Spotlight
//     //float theta = dot( L, spotLightBeamDir );
//     //float intensity = 1.0;
//     //float outerCutOff = 0.84;
//     //if ( theta > outerCutOff ) {
//     //	// spotlight
//     //	float epsilon = 0.90 - outerCutOff;
//     //	intensity = clamp( ( theta - outerCutOff ) / epsilon, 0.0, 1.0 );
//     //	// Attenuation
//     //	float distance = length( plmfrag );
//     //	float attenuation = 1.0 / ( u_pointLightAttenuation[i].x + u_pointLightAttenuation[i].y * distance + u_pointLightAttenuation[i].z * ( distance * distance ) );
//     //	diffuseValue += max( dot( N, L ) * intensity * u_pointLightIntensity[i].x * attenuation, 0.0 );
//     //}

//     // Pointlight
//     float distance = length( plmfrag );
//     float attenuation = 1.0 / ( u_pointLightAttenuation[i].x + u_pointLightAttenuation[i].y * distance + u_pointLightAttenuation[i].z * ( distance * distance ) );
//     float radiance = u_pointLightIntensity[i].x * attenuation;// *pow( visibility, 4 );

//     Lo += rendering_equation( albedo, L, V, N, F0, radiance );
// }

//    Lo *= v_color;

#end_code

#define_code shadow_code
float visibility = 0.0;
    // if ( dot( N, normalize( u_sunPosition - Position_worldspace ) ) > 0.25 )  {
    for ( int i = 0; i < 4; i++ ) {
        int index = i;// int( 16.0*random( vec4( gl_FragCoord.xyy, i ) ) ) % 16;
        visibility += texture( shadowMapTexture, vec3( v_shadowmap_coord3.xy + poissonDisk[index] / 4096.0, v_shadowmap_coord3.z ) ) * 0.25;// * u_timeOfTheDay;
    }
    // }
#end_code

#define_code final_combine

float ndotl = max(dot(N, V), 0.0);
vec3 F = fresnelSchlickRoughness(ndotl, F0, roughness);
//vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);

vec3 kS = F;
vec3 kD = 1.0 - kS;
kD *= 1.0 - metallic;

vec3 irradiance = texture(ibl_irradianceMap, N).rgb;
// float gr = irradiance.r * 0.3 + irradiance.g * 0.59 + irradiance.b * 0.11;
// irradiance.rgb = vec3(gr);
vec3 aoLightmapColor = texture(lightmapTexture, v_texCoord2).rgb;
vec3 diffuseV = Lo + (irradiance * albedo );

// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
const float MAX_REFLECTION_LOD = 6.0;
vec3 R = reflect(-V, N);
vec3 prefilteredColor = textureLod(ibl_specularMap, R, roughness * MAX_REFLECTION_LOD).rgb;
// gr = prefilteredColor.r * 0.3 + prefilteredColor.g * 0.59 + prefilteredColor.b * 0.11;
// prefilteredColor.rgb = vec3(gr);
vec2 brdf  = texture(ibl_brdfLUTMap, vec2( ndotl, roughness)).rg;
vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
specular = pow(specular, vec3(2.2/1.0)); 

vec3 ambient = (kD * diffuseV + specular) * visibility* ao;//* visibility// * pow(aoLightmapColor, vec3(8.2));// * visibility;//;
// vec3 ambient = (kD );

// vec3 finalColor = (Lo * visibility) + ambient; 
vec3 finalColor = ambient;//pow(aoLightmapColor, vec3(8.2));//N*0.5+0.5;//v_texCoord.xyx;//;//prefilteredColor;//vec3(brdf, 1.0);//ambient;//vec3(texture(metallicTexture, v_texCoord).rrr);//(N + vec3(1.0) ) * vec3(0.5);;//irradiance;// ambient;// prefilteredColor;//(V + vec3(1.0) ) * vec3(0.5);//ambient; //specular;//vec3(brdf.xy, 0.0);

// finalColor = finalColor / ( finalColor + vec3(1.00));
finalColor = vec3(1.0) - exp(-finalColor * 1.0);
//finalColor = pow(finalColor, vec3(2.2/1.0));

FragColor = vec4( finalColor, opacity * alpha ); 

//	BloomColor = vec4( ( incandescenceColor * incandescenceFactor ) + max(visibility-1.7, 0.0), 1.0 );
BloomColor = vec4( ( incandescenceColor * incandescenceFactor * finalColor ), 1.0 );
//        BloomColor = vec4( finalColor*2, 1.0 );

#end_code
