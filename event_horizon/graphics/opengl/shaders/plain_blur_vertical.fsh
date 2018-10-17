#version #opengl_version
       out vec4 FragColor;
       in vec2 v_texCoord;

       uniform sampler2D colorFBTexture;
       float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

       void main()
       {
           vec2 tex_offset = vec2(1.0) / vec2(textureSize(colorFBTexture, 0)); // gets size of single texel
           vec4 result = texture(colorFBTexture, v_texCoord) * weight[0]; // current fragment's contribution

           for(int i = 1; i < 5; ++i)
           {
               result += texture(colorFBTexture, v_texCoord + vec2(0.0, tex_offset.y * float(i))) * weight[i];
               result += texture(colorFBTexture, v_texCoord - vec2(0.0, tex_offset.y * float(i))) * weight[i];
           }

           FragColor = vec4( result.xyz, 1.0 );
       }
    