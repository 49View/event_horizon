#version #opengl_version
#precision_high

#include "camera_uniforms.glsl"

// layout(location = 0) out float FragDepth;
layout(location = 0) out vec2 FragDepth;
// out vec4 FragDepth;

float linearize(float depth) {
    float znear = u_nearFar.x;
    float zfar = u_nearFar.y;
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

void main() {
    // FragDepth = linearize(gl_FragCoord.z);
    // FragDepth = vec4(vec3(linearize(gl_FragCoord.z)), 1.0);
    FragDepth = vec2(linearize(gl_FragCoord.z));
    // FragDepth = linearize(gl_FragCoord.z);
    //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
    // FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
}
    