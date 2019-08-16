#version #opengl_version

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

float linearize(float depth) {
    float znear = u_nearFar.x;
    float zfar = u_nearFar.y;
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

vec3 bloom( float amount ) {
    return vec3(texture(bloomTexture, v_texCoord).r*amount);
}

vec3 ssao() {
    return texture(ssaoMapTexture, v_texCoord).xyz;
}

#include "dof.glsl"

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);
    // sceneColor.xyz = dof();

    // sceneColor.xyz += bloom(0.35); 
    //sceneColor.xyz *= ssao(); 
    sceneColor.xyz *= vignetting();
    sceneColor.xyz *= grain();

    FragColor = sceneColor;
}
