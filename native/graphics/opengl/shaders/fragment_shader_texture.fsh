#version #opengl_version

varying vec2 v_texCoord;

uniform sampler2D colorTexture;
#include "color_uniforms.glsl"

void main()
{
    vec4 textureCol = texture2D(colorTexture, v_texCoord);
    float preMultAlpha = min(alpha, textureCol.a*opacity);
    gl_FragColor = vec4( textureCol.rgb * diffuseColor * preMultAlpha, preMultAlpha);
}
