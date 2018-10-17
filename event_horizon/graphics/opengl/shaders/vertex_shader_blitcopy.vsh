#include "layout_pos_tex2d.glsl"

       void main() {
       v_texCoord = a_t1;
       gl_Position = vec4( a_position.x, a_position.y, 0.0, 1.0 );
       }
    