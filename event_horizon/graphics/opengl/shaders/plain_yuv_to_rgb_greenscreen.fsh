#version #opengl_version

in vec2 v_texCoord;

uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;
uniform float opacity;
uniform float alpha;
uniform vec3 diffuseColor;

out vec4 FragColor;

void main()
{
	float y = texture(yTexture, v_texCoord).r;
	float u = texture(uTexture, v_texCoord).r;
	float v = texture(vTexture, v_texCoord).r;
	
   //Convert yuv to rgb
	float cb = u;
	float cr = v;

	vec3 colorRGB;
	colorRGB.r  = clamp((y + 1.402 * (cr - 0.5)), 0.0, 1.0);
	colorRGB.g =  clamp((y - 0.344 * (cb - 0.5) - 0.714 * (cr - 0.5)), 0.0, 1.0);
    colorRGB.b =  clamp((y + 1.772 * (cb - 0.5)), 0.0, 1.0);
	
//	colorRGB = pow(colorRGB, vec3(2.2));
	
	float opacity = 1.0;
	if (colorRGB.g > 0.9 && colorRGB.r < 0.5 && colorRGB.b < 0.5) opacity = 0.0;
    // FragColor = vec4(colorRGB, opacity * alpha);
    FragColor = vec4(y, y, y, opacity * alpha);
}
																													
