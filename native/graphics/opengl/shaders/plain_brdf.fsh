#version #opengl_version
        varying vec2 v_texCoord;


       const float PI = 3.14159265359;
       // ----------------------------------------------------------------------------
       // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
       // efficient VanDerCorpus calculation.
       float RadicalInverse_VdC(int bits)
       {
           // ###WEBGL1###
           // I've killed this as  bit-wise operator supported in GLSL ES 3.00 and above only
//            bits = (bits << 16) | (bits >> 16);
//            bits = ((bits & 0x55555555) << 1) | ((bits & 0xAAAAAAAA) >> 1);
//            bits = ((bits & 0x33333333) << 2) | ((bits & 0xCCCCCCCC) >> 2);
//            bits = ((bits & 0x0F0F0F0F) << 4) | ((bits & 0xF0F0F0F0) >> 4);
//            bits = ((bits & 0x00FF00FF) << 8) | ((bits & 0xFF00FF00) >> 8);
//            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
           return 1.0;
       }
       // ----------------------------------------------------------------------------
       vec2 Hammersley(int i, int N)
       {
           return vec2(float(i)/float(N), RadicalInverse_VdC(i));
       }
       // ----------------------------------------------------------------------------
       vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
       {
           float a = roughness*roughness;

           float phi = 2.0 * PI * Xi.x;
           float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
           float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

           // from spherical coordinates to cartesian coordinates - halfway vector
           vec3 H;
           H.x = cos(phi) * sinTheta;
           H.y = sin(phi) * sinTheta;
           H.z = cosTheta;

           // from tangent-space H vector to world-space sample vector
           vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
           vec3 tangent   = normalize(cross(up, N));
           vec3 bitangent = cross(N, tangent);

           vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
           return normalize(sampleVec);
       }
       // ----------------------------------------------------------------------------
       float GeometrySchlickGGX(float NdotV, float roughness)
       {
           // note that we use a different k for IBL
           float a = roughness;
           float k = (a * a) / 2.0;

           float nom   = NdotV;
           float denom = NdotV * (1.0 - k) + k;

           return nom / denom;
       }
       // ----------------------------------------------------------------------------
       float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
       {
           float NdotV = max(dot(N, V), 0.0);
           float NdotL = max(dot(N, L), 0.0);
           float ggx2 = GeometrySchlickGGX(NdotV, roughness);
           float ggx1 = GeometrySchlickGGX(NdotL, roughness);

           return ggx1 * ggx2;
       }
       // ----------------------------------------------------------------------------
       vec2 IntegrateBRDF(float NdotV, float roughness)
       {
           vec3 V;
           V.x = sqrt(1.0 - NdotV*NdotV);
           V.y = 0.0;
           V.z = NdotV;

           float A = 0.0;
           float B = 0.0;

           vec3 N = vec3(0.0, 0.0, 1.0);

           const int SAMPLE_COUNT = 1024;
           for(int i = 0; i < SAMPLE_COUNT; ++i)
           {
               // generates a sample vector that's biased towards the
               // preferred alignment direction (importance sampling).
               vec2 Xi = Hammersley(i, SAMPLE_COUNT);
               vec3 H = ImportanceSampleGGX(Xi, N, roughness);
               vec3 L = normalize(2.0 * dot(V, H) * H - V);

               float NdotL = max(L.z, 0.0);
               float NdotH = max(H.z, 0.0);
               float VdotH = max(dot(V, H), 0.0);

               if(NdotL > 0.0)
               {
                   float G = GeometrySmith(N, V, L, roughness);
                   float G_Vis = (G * VdotH) / (NdotH * NdotV);
                   float Fc = pow(1.0 - VdotH, 5.0);

                   A += (1.0 - Fc) * G_Vis;
                   B += Fc * G_Vis;
               }
           }
           A /= float(SAMPLE_COUNT);
           B /= float(SAMPLE_COUNT);
           return vec2(A, B);
       }
       // ----------------------------------------------------------------------------
       void main()
       {
           vec2 integratedBRDF = IntegrateBRDF(v_texCoord.x, v_texCoord.y);
           // ###WEBGL1###
           // Double check we are returning a 2 channel texture
           gl_FragColor = integratedBRDF.xyxy;
       }
    