#include "plain_sh_common.glsl"
#include "texture_notile.glsl"

void main() {

   vec3 V = normalize( u_eyePos - Position_worldspace );
   vec3 vtang = normalize( tangentViewPos - tangentFragPos );

   vec2 position = v_texCoord2;
   vec2 size = vec2(1.0/16.0, 1.0/16.0);

   vec2 texCoords = position + size * fract(v_texCoord);

   ao = texture( aoTexture, texCoords ).x * aoV;
   roughness = (texture( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture( metallicTexture, texCoords ).x * metallicV);
   opacityV = texture( opacityTexture, texCoords ).x * opacity; 
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor;
   translucencyV = texture( translucencyTexture, texCoords ).x;
   albedo = pow(albedo, vec3(2.2/1.0));

   vec3 N = getNormalFromMap(texCoords);

   light_code

   final_combine_no_reflections
}