#include "layout_pos_tex3d.glsl"

       void main()
       {
       v_texCoord = a_t1;
       gl_Position = vec4( a_position, 1.0 );
       }
    