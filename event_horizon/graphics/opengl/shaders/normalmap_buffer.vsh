#include "layout_sh.glsl"
#include "camera_uniforms.glsl"

uniform mat4 u_modelMatrix;

void main() {
    mat3 modelMatrix3x3 = mat3( u_modelMatrix );
    mat3 viewMatrix3x3 = mat3( u_viewMatrix );
    modelMatrix3x3 = viewMatrix3x3 * modelMatrix3x3;
    modelMatrix3x3[0] = normalize( modelMatrix3x3[0] );
    modelMatrix3x3[1] = normalize( modelMatrix3x3[1] );
    modelMatrix3x3[2] = normalize( modelMatrix3x3[2] );
    //modelMatrix3x3 = inverse(modelMatrix3x3);
    v_norm = normalize( modelMatrix3x3 * a_t2 );
    gl_Position = u_mvpMatrix * u_modelMatrix * vec4( a_position, 1.0 );
}
