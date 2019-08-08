#version #opengl_version

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
// uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;


void main() {
    FragColor = texture(colorFBTexture, v_texCoord);
}
