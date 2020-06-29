#version #opengl_version

#include "color_uniforms.glsl"

layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 FragAttach1;

void main()
{
    float preMultAlpha = alpha*opacity;
    FragColor = vec4(diffuseColor * preMultAlpha, preMultAlpha);
}
