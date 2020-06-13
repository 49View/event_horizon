#version #opengl_version
varying vec3 v_texCoord;

uniform sampler2D colorTexture;

#include "lighting_uniforms.glsl"

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_texCoord));
    vec4 color = texture2D(colorTexture, uv);

    gl_FragColor = vec4(vec3(1.0) - exp(-color.xyz * u_sunRadiance.xyz), 1.0);
}
    