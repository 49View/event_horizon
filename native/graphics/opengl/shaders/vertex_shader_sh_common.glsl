#include "layout_sh.glsl"
#include "camera_uniforms.glsl"
#include "lighting_uniforms.glsl"
#include "animation_uniforms.glsl"
uniform mat4 u_modelMatrix;

mat3 shVertexData() {
   vec4 pos4 = vec4( a_position, 1.0 );
   v_texCoord = a_t1;
   v_texCoord2 = a_tuv2;

   //	mat3 modelMatrix3x3 = mat3(transpose(inverse(u_modelMatrix)));//mat3( u_modelMatrix );
   mat3 modelMatrix3x3 = mat3( u_modelMatrix );

   modelMatrix3x3[0] = normalize( modelMatrix3x3[0] );
   modelMatrix3x3[1] = normalize( modelMatrix3x3[1] );
   modelMatrix3x3[2] = normalize( modelMatrix3x3[2] );

   // Position of the vertex, attribute worldspace : M * position
   Position_worldspace = ( u_modelMatrix * pos4 ).xyz;

   // Shadowmap
   vec4 v_shadowmap_coord = u_depthBiasMVP * vec4( Position_worldspace, 1.0 );
   v_shadowmap_coord3 = vec3( v_shadowmap_coord.xy, v_shadowmap_coord.z / v_shadowmap_coord.w );

   gl_Position = u_mvpMatrix * u_modelMatrix * pos4;

//    v_color = vec3( 0.0 );
//    v_color.r += dot( shLightCoeffs[0], shR1 );
//    v_color.r += dot( shLightCoeffs[1], shR2 );
//    v_color.r += dot( shLightCoeffs[2], shR3 );

//    v_color.g += dot( shLightCoeffs[0], shG1 );
//    v_color.g += dot( shLightCoeffs[1], shG2 );
//    v_color.g += dot( shLightCoeffs[2], shG3 );

//    v_color.b += dot( shLightCoeffs[0], shB1 );
//    v_color.b += dot( shLightCoeffs[1], shB2 );
//    v_color.b += dot( shLightCoeffs[2], shB3 );

   // v_t8 = a_t8;

   return modelMatrix3x3;
}
    