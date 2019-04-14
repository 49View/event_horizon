#include "vertex_shader_sh_common.glsl"

void main() {
       mat3 modelMatrix3x3 = shVertexData();

       v_norm = normalize( modelMatrix3x3 * a_t2 );
       v_tan = normalize( modelMatrix3x3 * a_t3.xyz );
       v_bitan = normalize( modelMatrix3x3 * a_t4 );
       v_t8 = a_t8;

       // Check handness
       //	if (dot(cross(v_norm, v_tan), v_bitan) < 0.0) {
       //		v_tan *= -1.0;
       //	}
       mat3 itbn;
       itbn[0] = v_tan;
       itbn[1] = v_bitan;
       itbn[2] = v_norm;
       itbn = transpose( itbn );
}
