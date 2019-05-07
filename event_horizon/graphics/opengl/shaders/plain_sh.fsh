#include "plain_sh_common.glsl"
#include "texture_notile.glsl"

float edgeFactorTri() {
      vec3 d = fwidth( v_t8.xyz );
      vec3 a3 = smoothstep( vec3( 0.0 ), d * 1.5, v_t8.xyz );
      return min( min( a3.x, a3.y ), a3.z );
}

void main() {

//   mat3 itbn;
//   itbn[0] = normalize( v_tan );
//   itbn[1] = normalize( v_bitan );
//   itbn[2] = normalize( v_norm );
//   itbn = transpose( itbn );

   vec3 V = normalize( u_eyePos - Position_worldspace );

//   vec3 te = itbn * u_eyePos;
//   vec3 tw = itbn * Position_worldspace;
   
   vec2 texCoords = v_texCoord;//ParallaxMapping(v_texCoord, V);

   ao = texture( aoTexture, texCoords ).x * aoV;
   roughness = (texture( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture( metallicTexture, texCoords ).x * metallicV);
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor;
    albedo = pow(albedo, vec3(2.2/1.0));
    // albedo = pow(albedo, vec3(1.0/2.2));
//    albedo *= diffuseColor;
   //roughness = pow(roughness, 1.0/2.2);

   vec3 N = getNormalFromMap(texCoords);

   shadow_code

   light_code

   final_combine

      //sunDir = vec3(1);
//      vec3 LD = normalize( u_sunPosition - Position_worldspace );
      //vec3 HF = normalize( LD + V );

        //float spec = pow(max(dot(N, HF), 0.0), 100.0);
        //float diff = max(dot(N, LD), 0.0);
        //finalColor = vec3(spec + diff);
        //FragColor = vec4( finalColor, opacity * alpha );

}
    