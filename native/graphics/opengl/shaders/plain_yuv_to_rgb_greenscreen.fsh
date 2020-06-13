#version #opengl_version

varying vec2 v_texCoord;

uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;
#include "color_uniforms.glsl"

void main()
{
	float y = texture2D(yTexture, v_texCoord).r;
	float u = texture2D(uTexture, v_texCoord).r;
	float v = texture2D(vTexture, v_texCoord).r;
	
    //Convert yuv to rgb 
	float cb = u;
	float cr = v;

	vec3 colorRGB = diffuseColor;
	colorRGB.r  = clamp((y + 1.402 * (cr - 0.5)), 0.0, 1.0);
	colorRGB.g =  clamp((y - 0.344 * (cb - 0.5) - 0.714 * (cr - 0.5)), 0.0, 1.0);
    colorRGB.b =  clamp((y + 1.772 * (cb - 0.5)), 0.0, 1.0);
	
	float greenMod = pow(colorRGB.g, 1.0/2.2);
	
	float lopacity = 1.0;
	bool bCoords = false; //v_texCoord.x < 0.1 || v_texCoord.x > 0.8;
	if ( bCoords || (greenMod > 0.59 && colorRGB.r < 0.3 && colorRGB.b < 0.7 ) ) { // 
		lopacity = 0.0; 
	}
    // FragColor = vec4(colorRGB, opacity * alpha);
    gl_FragColor = vec4(colorRGB, lopacity * opacity * alpha);
}
																													
