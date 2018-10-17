#include "plain_sh_common.glsl"

       #define sh_notexture

       void main() {
           ao = aoV;
           roughness = roughnessV;
           metallic = metallicV;
           vec3 albedo = diffuseColor;
           vec3 N = normalize( v_norm );
           vec3 V = normalize( u_eyePos - Position_worldspace );

           shadow_code

           light_code

           final_combine
       }
    