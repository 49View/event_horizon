#version #opengl_version

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D colorTexture;
#include "color_uniforms.glsl"

void main()
{
    vec4 textureCol = texture(colorTexture, v_texCoord);
    FragColor = vec4( textureCol.rgb * diffuseColor, min(alpha, textureCol.a*opacity));
}
    