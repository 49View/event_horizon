#include "vertex_shader_sh_common.glsl"

void main() {
mat3 modelMatrix3x3 = shVertexData();

v_norm = normalize( modelMatrix3x3 * a_t2 );
}
    