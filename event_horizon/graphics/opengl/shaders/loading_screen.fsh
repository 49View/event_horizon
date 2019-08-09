#version #opengl_version
#include "animation_uniforms.glsl"
in vec2 v_texCoord;
out vec4 fragColor;
vec2 iResolution = vec2(640.0, 360.0);

float inRect(vec2 pos, vec2 topLeft, vec2 rightBottom) {
	return step(topLeft.x, pos.x) * step(rightBottom.y, pos.y) * step(-rightBottom.x, -pos.x) * step(-topLeft.y, -pos.y);
}

float inBetween(float x, float a, float b) {
    return step(a, x) * step(-b, -x);
}

vec3 loadingColor( vec2 uv, float progress ) {
    vec2 inv_resolution = 1.0 / iResolution.xy;
	float sWidth = iResolution.x * inv_resolution.y;
    const float barWidthRatio = 0.7;
    float inv_barWidth = 1.0 / (barWidthRatio * sWidth);
    float barHeight = 0.0070;
    float twice_inv_barHeight = 2.0 / barHeight;
    uv.x = uv.x * sWidth;

    mat3 T_bar2s = mat3(
        vec3(inv_barWidth, 0.0, 0.0),
        vec3(0.0, inv_barWidth, 0.0),
        vec3((1.0 - sWidth * inv_barWidth) * 0.5, -0.48 * inv_barWidth, 1.0)
    );

    vec2 uv_bar = (T_bar2s * vec3(uv.xy, 1.0)).xy;
    float isInBaseRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(1.0, -0.5 * barHeight));
    float isInActiveRect = inRect(uv_bar, vec2(0.0, 0.5 * barHeight), vec2(progress, -0.5 * barHeight));
    vec3 baseColor = vec3(0.12941, 0.13725, 0.17647);
    vec3 activeColor = mix(vec3(0.2, 0.35294, 0.91373), vec3(0.43529, 0.43529, 0.96078), uv_bar.x);
    vec3 color = vec3(0.0, 0.0, 0.0);
    color = mix(color, baseColor, isInBaseRect);
    color = mix(color, activeColor, isInActiveRect);

	return color;
}

void main() {

    float iTime = u_deltaAnimTime.y;
    float progress = u_deltaAnimTime.w;
    vec2 uv = v_texCoord;

    float lowerLimit = 0.2;
    float higherLimit = 1.0f - lowerLimit;
    float alphaing = progress < lowerLimit ? progress : 1.0;

    if ( progress < lowerLimit ) {
    	alphaing = smoothstep(0.0f, lowerLimit, progress);
    } else if ( progress > higherLimit ) {
    	alphaing = 1.0 - smoothstep(higherLimit, 1.0f, progress);
    }

    vec4 c = vec4(loadingColor(uv, progress), 0.0);
    c *= alphaing;

    fragColor = c;
}
