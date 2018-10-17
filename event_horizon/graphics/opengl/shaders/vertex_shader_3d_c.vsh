#include "layout_pos_tex3d.glsl"
#include "camera_uniforms.glsl"

uniform mat4 u_modelMatrix;

void main() {
    gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position, 1.0 );
}
