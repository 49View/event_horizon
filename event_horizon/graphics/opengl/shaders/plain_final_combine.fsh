#version #opengl_version

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
// uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;

void main()
{
    FragColor = texture(colorFBTexture, v_texCoord);
    // FragColor = vec4(0.0);

    // vec4 bloomColor = texture(bloomTexture, v_texCoord);

    // float shadowColor = texture(shadowMapTexture, v_texCoord).r;
    // vec3 shadowMask = vec3(shadowColor, shadowColor,shadowColor);
    // FragColor = sceneColor + vec4( shadowMask, 1.0 ) * 0.1;

    // FragColor = vec4( mix(shadowMask, ( sceneColor.xyz ), vec3(0.5)), 1.0);

    // FragColor = sceneColor;
    // FragColor = sceneColor + bloomColor;
    //FragColor.xyz /= ( FragColor.xyz + vec3( 1.0 ) );

    // vec2 uv = v_texCoord;
    // uv *=  1.0 - uv.yx; 
    // float vig = uv.x*uv.y * 2500.0; // multiply with sth for intensity
    // vig = clamp( pow(vig, 2.95), 0.0, 1.0); // change pow for modifying the extend of the  vignette

    // FragColor = vec4( mix( vec3(1.0), pow( FragColor.xyz, vec3( 1.0 / 2.2 ) ), vig), FragColor.a );
}
