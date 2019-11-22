#include "layout_pos3d.glsl"
#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"

uniform mat4 u_modelMatrix;

void main() {
    gl_Position = u_mvpShadowMap * u_modelMatrix * vec4( a_position, 1.0 );
//      gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position, 1.0 );
}
