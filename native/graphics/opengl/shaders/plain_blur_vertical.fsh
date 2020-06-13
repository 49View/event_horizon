#version #opengl_version
varying vec2 v_texCoord;

uniform sampler2D colorFBTexture;

void main()
{
    float weight[5];
    weight[0] = 0.227027;
    weight[1] = 0.1945946;
    weight[2] = 0.1216216;
    weight[3] = 0.054054;
    weight[4] = 0.016216;

    // ###WEBGL1###
    // textureSize(colorFBTexture, 0) not avaiable, hardcode the side
    //    vec2 tex_offset = vec2(1.0) / vec2(textureSize(colorFBTexture, 0)); // gets size of single texel
    vec2 tex_offset = vec2(1.0) / vec2(640.0, 480.0); // gets size of single texel
    vec4 result = texture2D(colorFBTexture, v_texCoord) * weight[0]; // current fragment's contribution

    for(int i = 1; i < 5; ++i)
    {
        result += texture2D(colorFBTexture, v_texCoord + vec2(0.0, tex_offset.y * float(i))) * weight[i];
        result += texture2D(colorFBTexture, v_texCoord - vec2(0.0, tex_offset.y * float(i))) * weight[i];
    }

    gl_FragColor = vec4( result.xyz, 1.0 );
}
