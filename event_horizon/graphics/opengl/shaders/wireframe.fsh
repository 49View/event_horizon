#include "plain_sh_common.glsl"
#include "texture_notile.glsl"

float edgeFactorTri() {
      vec3 d = fwidth( v_t8.xyz );
      vec3 a3 = smoothstep( vec3( 0.0 ), d * 1.5, v_t8.xyz );
      return min( min( a3.x, a3.y ), a3.z );
}

void main() {
   mat3 itbn;
   itbn[0] = normalize( v_tan ); 
   itbn[1] = normalize( v_bitan );
   itbn[2] = normalize( v_norm );
   itbn = transpose( itbn );

   vec3 V = normalize( u_eyePos - Position_worldspace );

   vec3 te = itbn * u_eyePos;
   vec3 tw = itbn * Position_worldspace;
   vec2 texCoords = v_texCoord;//ParallaxMapping(v_texCoord, V);

   ao = texture( aoTexture, texCoords ).x * aoV;
   roughness = texture( roughnessTexture, texCoords ).x * roughnessV;
   metallic = texture( metallicTexture, texCoords ).x * metallicV;
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor;

   //vec4 normalCol = texture( normalTexture, texCoords );
   //vec3 N = normalize( normalCol.rgb*2.0 - 1.0 );
   vec3 N = getNormalFromMap(texCoords);

   shadow_code

   light_code

   final_combine

   FragColor.rgb = mix( vec3( 1.0 ), vec3( FragColor.rgb ), edgeFactorTri() );
   FragColor.a = 0.5;
   //BloomColor = vec4(1.0);
}
