#include "plain_sh_common.glsl"
#include "texture_notile.glsl"

void main() {

   vec3 V = normalize( u_eyePos - Position_worldspace );
   vec3 vtang = normalize( tangentViewPos - tangentFragPos );

   vec2 texCoords = v_texCoord;

   ao = texture( aoTexture, texCoords ).x * aoV;
   roughness = (texture( roughnessTexture, texCoords ).x * roughnessV);
   metallic = (texture( metallicTexture, texCoords ).x * metallicV);
   opacityV = texture( opacityTexture, texCoords ).x * opacity; 
   vec3 albedo = texture( diffuseTexture, texCoords ).xyz * diffuseColor;
   translucencyV = texture( translucencyTexture, texCoords ).x;

   vec3 N = getNormalFromMap(texCoords);

   shadow_code

   light_code

   final_combine_lightmap
}
