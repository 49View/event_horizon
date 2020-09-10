#include "plain_sh_common.glsl"
#include "texture_notile.glsl"
// #define sh_notexture

void main() {

   vec3 V = normalize( u_eyePos - Position_worldspace );

   vec2 texCoords = v_texCoord;//ParallaxMapping(v_texCoord, V);
 
   ao = texture( aoTexture, texCoords ).x * aoV; 
   roughness = (texture( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture( metallicTexture, texCoords ).x * metallicV);
   opacityV = texture( opacityTexture, texCoords ).x; // * 1.0 - 0.0 ) * opacity; 
   translucencyV = (texture( translucencyTexture, texCoords ).x);
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor*1.3183;
   albedo = pow(albedo, vec3(2.2/1.0));

   vec3 N = getNormalFromMap(texCoords);

   shadow_code

   light_code 

   final_combine
}
    