#version #opengl_version
#precision_high

#define _CAMERA_MOTION_BLURRING_ 0
#define _VIGNETTING_ 1
#define _GRAINING_ 1
#define _DOFING_ 0
#define _LUT3DING_ 0
#define _BLOOMING_ 0
#define _SSAOING_ 1

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"
#include "animation_uniforms.glsl"

in vec2 v_texCoord;
in vec3 v_texT2;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;
uniform sampler3D lut3dTexture;
uniform sampler2D depthMapTexture;
uniform sampler2D ssaoMapTexture;
uniform sampler2D uiTexture;

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

vec3 cameraMotionBlur( vec3 sceneColor ) {
    vec4 current = vec4(v_texT2 * texture(depthMapTexture, v_texCoord).r, 1.0);
    current = u_inverseMvMatrix * current; 
    vec4 previous = u_prevMvpMatrix * vec4(current.xyz, 1.0);
    previous.xy /= -previous.w;
    previous.xy = previous.xy * vec2(-0.5, 0.5) + 0.5;
    float mblurScale = (1.0/u_deltaAnimTime.x) / 60.0;
    vec2 blurVec = (previous.xy - v_texCoord) * mblurScale;

    const int nSamples = 4;
    for (int i = 1; i < nSamples; ++i) {
        vec2 offset = blurVec * (float(i) / float(nSamples - 1) - 0.5);  
        sceneColor.xyz += texture(colorFBTexture, v_texCoord + offset ).xyz;
    }
 
    sceneColor.xyz /= float(nSamples);
    return sceneColor;
}

#include "dof.glsl"

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);

    // sceneColor = texture(shadowMapTexture, v_texCoord).xxxx;
    // FragColor.rgb= sceneColor.rgb;
    // FragColor.a = 1.0;
    // return;

    #if _CAMERA_MOTION_BLURRING_
    sceneColor.xyz = cameraMotionBlur(sceneColor.xyz);
    #endif

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
    sceneColor.xyz *= pow( ssao(), vec3(1.5)); //try with some exp on ssao, might look better
    #endif

    #if _VIGNETTING_
    sceneColor.xyz *= vignetting();
    #endif

    #if _GRAINING_
    sceneColor.xyz *= grain();
    #endif

    vec4 uiColor = texture(uiTexture, v_texCoord);
    sceneColor.rgb = mix(sceneColor.rgb, uiColor.rgb, uiColor.a);

    FragColor = sceneColor;
}
