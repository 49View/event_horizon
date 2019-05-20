#version #opengl_version

#include "color_uniforms.glsl"

out vec4 FragColor;

void main()
{
    FragColor = vec4(diffuseColor, alpha*opacity);
}
 