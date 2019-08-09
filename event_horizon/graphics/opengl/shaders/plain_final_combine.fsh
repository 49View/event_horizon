#version #opengl_version

#include "lighting_uniforms.glsl"

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
// uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;
uniform sampler3D lut3dTexture;

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

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);

    // sceneColor.xyz = vec3(1.0) - exp(-sceneColor.xyz * u_hdrExposures.x*0.55);

    // sceneColor.xyz = texture( lut3dTexture, sceneColor.xyz ).xyz;

    sceneColor.xyz *= vignetting();

    FragColor = sceneColor;
}
