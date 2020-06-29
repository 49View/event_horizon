#version #opengl_version

in vec2 v_texCoord;

layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 FragAttach1;

uniform sampler2D colorTexture;
#include "color_uniforms.glsl"

void main()
{
    vec4 textureCol = texture(colorTexture, v_texCoord);
    float preMultAlpha = min(alpha, textureCol.a*opacity);
    FragColor = vec4( textureCol.rgb * diffuseColor * preMultAlpha, preMultAlpha);
}
