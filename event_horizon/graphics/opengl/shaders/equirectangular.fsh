#version #opengl_version
in vec3 v_texCoord;
out vec4 color;

uniform sampler2D colorTexture;

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
    color = texture(colorTexture, uv);
    color.xyz = vec3(1.0) - exp(-color.xyz * 1.0);
}
    