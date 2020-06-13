#version #opengl_version
#precision_high
varying vec3 v_texCoord;

uniform samplerCube cubeMapTexture;
//uniform float delta;

void main()
{
    // color.xyz = mix( texture2D(cubeMapTexture, v_texCoord ).xyz, vec3(1.0), delta);
    gl_FragColor.xyz = textureCube(cubeMapTexture, v_texCoord ).xyz;
    gl_FragColor.w = 1.0;
//    BloomColor = vec4(FragColor.xyz, 1.0);
    // color = vec4( v_texCoord, 1.0);
}
    