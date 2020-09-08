#version #opengl_version
#precision_high

layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 FragAttach1;

in vec2 v_texCoord;
in vec2 v_texCoord2;
in vec3 v_norm;
in vec3 v_tan;
in vec3 v_bitan;
in vec4 v_color;
in vec3 v_shadowmap_coord3;
in vec3 Position_worldspace;
in vec3 tangentViewPos;
in vec3 tangentFragPos;
// in vec4 v_t8;

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
uniform sampler2D opacityTexture; 
uniform sampler2D translucencyTexture;
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
float opacityV = 1.0;
float translucencyV = 1.0;

const float PI = 3.14159265359;

float random(vec4 seed4){
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
    // return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 poissonDisk[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.294184101, -0.232938870 ),
    vec2( 0.394184101, 0.892938870 ),
    vec2( -0.794184101, -0.93938870 ),
    vec2( 0.494184101, 0.45938870 ),
    vec2( -0.694184101, -0.71938870 ),
    vec2( 0.594184101, 0.15938870 ),
    vec2( -0.294184101, -0.732938870 ),
    vec2( 0.194184101, 0.02938870 ),    
    vec2( -0.714184101, -0.55938870 ),
    vec2( -0.214184101, 0.39938870 ),
    vec2( -0.644184101, 0.89938870 ),
    vec2( 0.34495938, -0.29387760 )
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
    vec2 P = viewDir.xy / -viewDir.z * height_scale;
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
    vec3 tangentNormal = texture(normalTexture, texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(Position_worldspace);
    vec3 Q2  = dFdy(Position_worldspace);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords)+vec2(0.00000001, 0.00000001);

    vec3 N  = normalize(v_norm);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);

    // vec3 tangentNormal = texture(normalTexture, texCoords).xyz * 2.0 - 1.0;
    // mat3 TBN = mat3(v_tan, v_bitan, v_norm);
    // return normalize(TBN * tangentNormal);
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

vec3 rendering_equation( vec3 albedo, vec3 L, vec3 V, vec3 N, vec3 F0, vec3 radiance ) {
    vec3 H = normalize( L + V );
    float NdotL = max( dot( N, L ), 0.0 );

    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );

    float NDF = DistributionGGX( N, H, roughness );
    float G = GeometrySmith( N, V, L, roughness );

    vec3 nominator = NDF * G * F;
    float denominator = ( (1.0/radiance.r) * max( dot( N, V ), 0.0 ) * NdotL) + 0.01;
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

    vec3 L_Sun = u_sunDirection;// normalize( u_sunPosition - Position_worldspace );

    if ( u_sunDirection.y > 0.0 ) {
        vec3 H = -normalize( vec3(0.0, 1.0, 0.0) + u_sunDirection );
        // u_shadowParameters[2] == Indoor Scene Coeff
        vec3 sunColor = u_sunRadiance.xyz*u_shadowParameters[2];
        Lo += rendering_equation( albedo, L_Sun, V, N, F0, sunColor );
        Lo += rendering_equation( albedo, H, V, N, F0, sunColor*0.5 );
        if ( translucencyV > 0.0 ) {
            Lo += rendering_equation( albedo, -L_Sun, V, N, F0, sunColor );
        }
    }

    vec3 daiLightAmountTrick = vec3(3.5 * u_sunRadiance.w);
    // single point light 
    for ( int i = 0; i < u_numPointLights; i++ ) {
        vec3 plmfrag = vec3(u_pointLightPos[i]) - Position_worldspace;
        float pldistance = length( plmfrag );
        vec3 L = normalize( plmfrag );
        float plDistanceAtt = (pldistance*pldistance);
        vec3 lradiance = ( u_pointLightIntensity[i] + daiLightAmountTrick ) / plDistanceAtt;
        Lo += rendering_equation( albedo, L, V, N, F0, lradiance );
    }

#ifdef sh_reflections

#endif

#end_code

    // u_shadowParameters[0] == depth value z offset to avoid horrible aliasing
    // u_shadowParameters[1] == shadowOverBurn coefficient 
#define_code shadow_code
    float visibility = u_sunDirection.y > 0.0 ? 1.0 : 0.22;
    if ( u_sunDirection.y > 0.0 ) {
        vec3 shadowmap_coord3Biases = v_shadowmap_coord3;
        vec3 VN = v_norm;
        if ( translucencyV > 0.0 ) {
            VN *= -1.0;
        }
        float nlAngle = dot( VN, u_sunDirection);
        // float tanCosNAngle = tan(acos(nlAngle));
        // visibility += texture( shadowMapTexture, shadowmap_coord3Biases ) * u_shadowParameters[1];// * tan(acos(1.0-nlAngle));

        // if ( shadowmap_coord3Biases.z > 0.0 ) {
        if ( nlAngle > 0.3 ) {
            shadowmap_coord3Biases = clamp( shadowmap_coord3Biases, vec3(0.0), vec3(1.0) );
            shadowmap_coord3Biases.z -= u_shadowParameters[0]+0.00024;
            // float overBurnedfactor = 0.2;// * u_shadowParameters[1];
            for ( int i = 0; i < 4; i++ ) {
                int index = i;//int( 4.0*random( gl_FragCoord.xyyx ) ) % 4;        
                float shadow = texture( shadowMapTexture, vec3( shadowmap_coord3Biases.xy + (poissonDisk[index] / 2048.0), shadowmap_coord3Biases.z ) );// * u_timeOfTheDay;
                shadow = shadow < shadowmap_coord3Biases.z ? 1.0 : 0.0;
                visibility -= shadow * 0.195;// * u_timeOfTheDay;
            }
            // float shadow = texture( shadowMapTexture, vec3( shadowmap_coord3Biases.xy, shadowmap_coord3Biases.z ) );
            // shadow = shadow < shadowmap_coord3Biases.z ? 1.0 * 0.78 : 0.0;
            // visibility -=  shadow;
        } else {
            visibility -= 0.78;
        }
    }
    // }
    // visibility = pow(visibility, 2.0);
    //clamp(visibility, 0.0, 1.0);
    // visibility = nlAngle;
#end_code

#define_code final_combine
float ndotl = max(dot(N, V), 0.0);
vec3 F = fresnelSchlickRoughness(ndotl, F0, roughness);
//vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);

vec3 kS = F;
vec3 kD = 1.0 - kS;
kD *= 1.0 - metallic;

//vec3 aoLightmapColor = texture(lightmapTexture, v_texCoord2).rrr;


#ifdef sh_reflections
vec3 R = reflect(-V, N);

vec3 irradiance = texture(ibl_irradianceMap, N).rgb;
vec3 specular = vec3(0.0);
// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
vec3 diffuseV = (irradiance * albedo * v_color.rbg );// * aoLightmapColor;

const float MAX_REFLECTION_LOD = 5.0;

// vec3 prefilteredColor = textureLod(ibl_specularMap, R, 0.0 + (0.5 * MAX_REFLECTION_LOD)).rgb;
vec3 prefilteredColor = textureLod(ibl_specularMap, R, roughness*MAX_REFLECTION_LOD).rgb;
// vec3 prefilteredColor = texture(ibl_specularMap, R).rgb;
vec2 brdf = texture(ibl_brdfLUTMap, vec2( ndotl, roughness)).rg;
specular = prefilteredColor * (F * brdf.x + brdf.y);
// specular = pow(specular, vec3(2.2/1.0)); 
// vec3 ambient = u_sunRadiance.xyz;

vec3 ambient = (((Lo + (kD * diffuseV + specular)) * visibility ) * ao);// * (visibility+diffuseV);

#else 
vec3 ambient = Lo * ((kD * albedo * v_color.rgb ) * visibility); //kD * Lo * ao;// * visibility;
#endif

vec3 finalColor = ambient; //pow(aoLightmapColor, vec3(8.2));//N*0.5+0.5;//v_texCoord.xyx;//;//prefilteredColor;//vec3(brdf, 1.0);//ambient;//vec3(texture(metallicTexture, v_texCoord).rrr);//(N + vec3(1.0) ) * vec3(0.5);;//irradiance;// ambient;// prefilteredColor;//(V + vec3(1.0) ) * vec3(0.5);//ambient; //specular;//vec3(brdf.xy, 0.0);
float fogZ = length(u_eyePos-Position_worldspace);
float fog = 1.0-(smoothstep(u_nearFar[3],u_nearFar[1],fogZ)*1.0);

finalColor = vec3(1.0) - exp(-finalColor * u_hdrExposures.x);

float preMultAlpha = opacityV * alpha * fog;
FragColor = vec4( finalColor * preMultAlpha, preMultAlpha ); 

vec3 bloom = finalColor * (translucencyV*(visibility-1.0));
FragAttach1 = vec4( bloom, 1.0 );//vec4(gl_FragCoord.z);
//	BloomColor = vec4( ( incandescenceColor * incandescenceFactor ) + max(visibility-1.7, 0.0), 1.0 );
// BloomColor = vec4( ( incandescenceColor * incandescenceFactor * finalColor ), 1.0 );
//        BloomColor = vec4( finalColor*2, 1.0 );

#end_code
