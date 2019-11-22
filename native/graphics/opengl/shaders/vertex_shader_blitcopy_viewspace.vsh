#include "layout_pos_tex2d_v3.glsl"

void main() {
       v_texCoord = a_t1;
       v_texT2 = a_t2;
       gl_Position = vec4( a_position.x, a_position.y, 0.0, 1.0 );
}
    