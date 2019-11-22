#include "layout_pos3d.glsl"
#include "camera_uniforms.glsl"
uniform mat4 u_modelMatrix;

void main() {
       gl_Position = u_screenSpaceMatrix * u_modelMatrix * vec4( a_position, 1.0 );
}
    