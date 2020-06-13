#include "plain_sh_common.glsl"
#include "texture_notile.glsl"
#define sh_reflections

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

//   vec3 te = itbn * u_eyePos;
//   vec3 tw = itbn * Position_worldspace;
   
   vec2 texCoords = v_texCoord;
   // vec2 texCoords = ParallaxMapping(v_texCoord, V);

   ao = texture2D( aoTexture, texCoords ).x * aoV;
   roughness = (texture2D( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture2D( metallicTexture, texCoords ).x * metallicV);
   opacityV = texture2D( opacityTexture, texCoords ).x * opacity;
   vec3 albedo = texture2D( diffuseTexture, texCoords ).xyz * diffuseColor;
   albedo = pow(albedo, vec3(2.2/1.0));

   vec3 N = getNormalFromMap(texCoords);

   shadow_code
   translucencyV = texture2D( translucencyTexture, texCoords ).x;

   light_code

   final_combine
}
    