#version #opengl_version
#precision_high
in vec3 v_texCoord;

layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 BloomColor;

uniform samplerCube cubeMapTexture;
uniform float delta;

void main()
{
    // color.xyz = mix( texture(cubeMapTexture, v_texCoord ).xyz, vec3(1.0), delta);
    FragColor.xyz = texture(cubeMapTexture, v_texCoord ).xyz;
    FragColor.w = 1.0;
    BloomColor = vec4(0.0, 0.0, 0.0, 1.0);
    // color = vec4( v_texCoord, 1.0);
}
    