#include "plain_sh_common.glsl"
#include "texture_notile.glsl"

// float edgeFactorTri() {
//       vec3 d = fwidth( v_t8.xyz );
//       vec3 a3 = smoothstep( vec3( 0.0 ), d * 1.5, v_t8.xyz );
//       return min( min( a3.x, a3.y ), a3.z );
// }

void main() {

//   mat3 itbn;
//   itbn[0] = normalize( v_tan );
//   itbn[1] = normalize( v_bitan );
//   itbn[2] = normalize( v_norm );
//   itbn = transpose( itbn );

   vec3 V = normalize( u_eyePos - Position_worldspace );
   vec3 vtang = normalize( tangentViewPos - tangentFragPos );

   vec2 texCoords = v_texCoord;
   //vec2 texCoords = ParallaxMapping(v_texCoord, vtang);

   ao = texture( aoTexture, texCoords ).x * aoV;
   roughness = (texture( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture( metallicTexture, texCoords ).x * metallicV);
   opacityV = texture( opacityTexture, texCoords ).x * opacity; 
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor*1.3183;
   translucencyV = texture( translucencyTexture, texCoords ).x;
   albedo = pow(albedo, vec3(2.2/1.0));

   vec3 N = getNormalFromMap(texCoords);

   light_code

   final_combine_no_reflections
}
