#version #opengl_version
in vec3 v_texCoord;
out vec4 color;

uniform samplerCube cubeMapTexture;
uniform float delta;

void main()
{
    color.xyz = mix( texture(cubeMapTexture, v_texCoord ).xyz, vec3(1.0), delta);
    color.w = 1.0;
    // color = vec4( v_texCoord, 1.0);
}
    