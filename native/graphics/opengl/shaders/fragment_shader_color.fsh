#version #opengl_version

#include "color_uniforms.glsl"

void main()
{
    float preMultAlpha = alpha*opacity;
    gl_FragColor = vec4(diffuseColor * preMultAlpha, preMultAlpha);
}
