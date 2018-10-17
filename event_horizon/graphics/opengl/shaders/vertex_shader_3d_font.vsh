#include "layout_font.glsl"
       #include "camera_uniforms.glsl"

       uniform mat4 u_modelMatrix;

       void main() {
       tpos = vec3( a_t1.x*0.5, max( a_t1.x - 1.0, 0.0 ), a_t1.y );
       gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position.xyz, 1.0 );
       }
    