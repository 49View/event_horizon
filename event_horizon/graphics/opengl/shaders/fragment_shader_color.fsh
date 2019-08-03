#version #opengl_version

#include "color_uniforms.glsl"

out vec4 FragColor;

void main()
{
    float preMultAlpha = alpha*opacity;
    FragColor = vec4(diffuseColor * preMultAlpha, preMultAlpha);
}
