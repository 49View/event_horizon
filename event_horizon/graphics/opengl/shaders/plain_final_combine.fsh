#version #opengl_version

       in vec2 v_texCoord;
       out vec4 FragColor;
       uniform sampler2D colorFBTexture;
       uniform sampler2D bloomTexture;
       uniform sampler2D shadowMapTexture;

       void main()
       {
           vec4 sceneColor = texture(colorFBTexture, v_texCoord);
           // vec4 bloomColor = texture(bloomTexture, v_texCoord);

           //float shadowColor = texture(shadowMapTexture, v_texCoord).x;
           //vec3 shadowMask = vec3(shadowColor, shadowColor,shadowColor);
           //FragColor = vec4( shadowMask, 1.0 );
           //FragColor = vec4( mix(shadowMask, ( sceneColor.xyz ), vec3(0.5)), 1.0);

           FragColor = sceneColor;
           // FragColor = sceneColor + bloomColor;
           //FragColor.xyz /= ( FragColor.xyz + vec3( 1.0 ) );

           FragColor = vec4( pow( FragColor.xyz, vec3( 1.0 / 2.2 ) ), FragColor.a );

           //FragColor = sceneColor;
       }
    