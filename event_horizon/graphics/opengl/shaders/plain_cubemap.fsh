#version #opengl_version
in vec3 v_texCoord;
out vec4 color;

uniform samplerCube cubeMapTexture;

void main()
{
    color = vec4( texture(cubeMapTexture, v_texCoord ).xyz, 1.0);
    // color = vec4( v_texCoord, 1.0);
}
    