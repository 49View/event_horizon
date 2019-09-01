#version #opengl_version
#precision_high

#define _VIGNETTING_ 0
#define _GRAINING_ 0
#define _DOFING_ 0
#define _LUT3DING_ 0
#define _BLOOMING_ 0
#define _SSAOING_ 0

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;
uniform sampler3D lut3dTexture;
uniform sampler2D depthMapTexture;
uniform sampler2D ssaoMapTexture;

float vignetting() {
    // Vignetting
    float fstop = 0.9;
    float vignout = 1.3; // vignetting outer border
    float vignin = 0.0; // vignetting inner border
    float vignfade = 22.0; // f-stops till vignete fades
    float dist = distance(v_texCoord, vec2(0.5,0.5));
    dist = smoothstep(vignout+(fstop/vignfade), vignin+(fstop/vignfade), dist);
    return clamp(dist,0.0,1.0);
}

float grain() {
    float strength = 6.0;    
    float iTime = 1.748923;
    float x = (v_texCoord.x + 2.0 ) * (v_texCoord.y + 4.0 ) * (iTime * 10.0);
	return 1.0 - ((mod((mod(x, 17.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.02)-0.0075) * strength);
}

vec3 bloom( float amount ) {
    return vec3(texture(bloomTexture, v_texCoord).r*amount);
}

vec3 ssao() {
    return texture(ssaoMapTexture, v_texCoord).xyz;
}

vec3 lut3d( vec3 sceneColor ) {
    return texture( lut3dTexture, sceneColor ).xyz;
}

#include "dof.glsl"

float linearize2(float depth) {
    float znear = u_nearFar.x;
    float zfar = u_nearFar.y;
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);
    #if _DOFING_
    sceneColor.xyz = dof();
    #endif

    #if _LUT3DING_
    sceneColor.xyz = lut3d(sceneColor.xyz);
    #endif

    #if _BLOOMING_
    sceneColor.xyz += bloom(0.35); 
    #endif

    #if _SSAOING_
    sceneColor.xyz *= ssao(); 
    #endif

    #if _VIGNETTING_
    sceneColor.xyz *= vignetting();
    #endif

    #if _GRAINING_
    sceneColor.xyz *= grain();
    #endif

    // sceneColor.xyz = vec3(linearize2(texture(depthMapTexture, v_texCoord).r))/40;
    // sceneColor.xyz = texture(depthMapTexture, v_texCoord).rrr;
    // sceneColor.w = 1.0;
    FragColor = sceneColor;//*u_motionBlurParams[0];
}
