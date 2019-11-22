#include "layout_pos_texcubemap.glsl"
#include "camera_uniforms.glsl"

void main() {
    mat4 vm = u_projMatrix * u_viewMatrix;
    gl_Position = vm * vec4( a_position, 1.0 );
    v_texCoord = a_position.xyz;
}
    