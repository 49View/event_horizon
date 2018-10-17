#include "layout_pos_texcubemap.glsl"
#include "camera_uniforms.glsl"

void main() {
    mat4 vm_zerod = u_viewMatrix;
    vm_zerod[3] = vec4( 0.0, 0.0, 0.0, 1.0 );
    mat4 vm = u_projMatrix * vm_zerod;
    vec4 pos4 = vm * vec4( a_position, 1.0 );
    gl_Position = pos4.xyww;
    v_texCoord = a_position.xyz;
}
    