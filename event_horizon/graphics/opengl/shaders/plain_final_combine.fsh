#version #opengl_version

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
// uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;

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

    sceneColor.xyz *= vignetting();

    FragColor = sceneColor;
}
