#version #opengl_version

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"

in vec2 v_texCoord;
in vec3 v_texT2;
out vec4 FragColor;
uniform sampler2D depthMapTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D noise4x4Texture;

float ssao() {
    float occluded = 1.0;
    vec2 uNoiseScale = vec2(1024.0/4.0, 1024.0/4.0);
    vec3 origin = v_texT2 * texture(depthMapTexture, v_texCoord).r;

    // vec3 origin = v_texT2 * texture(depthMapTexture, v_texCoord).r;
    if ( -origin.z > 999.9 ) return 1.0;
    // return -origin.z/100.0;
    vec3 normal = texture(normalMapTexture, v_texCoord).xyz * 2.0 - 1.0;
    normal = normalize(normal);
    normal.y = -normal.y;
    vec3 rvec = texture(noise4x4Texture, v_texCoord * uNoiseScale).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    // float currZ = linearize(texture(depthMapTexture, v_texCoord).r);
    // for ( int t = 0; t < u_hemisphereSampleKernelSize; t++ ) {
    //     float sampledZ = linearize(texture(depthMapTexture, v_texCoord + (u_hemisphereSampleKernel[t].xy/15.0)*rvec.xy).r);
    //     if ( sampledZ < currZ && currZ - sampledZ < 0.2) { //
    //         occluded -= 1.0/u_hemisphereSampleKernelSize;
    //     }
    // }
    //return origin.z;

    float occlusion = 0.0;
    float uRadius = u_ssaoParameters[0]; //25.0 defaults;
    int numSamples = int(u_ssaoParameters[1]); // u_hemisphereSampleKernelSize
    for (int i = 0; i < numSamples; ++i) {
        // get sample position:
        vec3 sam = u_hemisphereSampleKernel[i];
        //sam.z = -sam.z;
        vec3 zsample = tbn * sam;
        zsample = zsample * uRadius + origin;
        // zsample = origin;
        
        // project zsample position:
        vec4 offset = vec4(zsample, 1.0);
        offset = u_projMatrix * offset;        
        offset.xy /= -offset.w;
        offset.xy = offset.xy * vec2(-0.5, 0.5) + 0.5;
        
        // get zsample depth:
        float sampleDepth = texture(depthMapTexture, offset.xy).r;
        // vec3 sampleDepth3 = u_eyeDir.xyz * linearize(texture(depthMapTexture, offset.xy).r);
        // return offset.y/50;        
        // return sampleDepth/4;
        //return sampleDepth - zsample.z;
        // return -zsample.z/400;
        // range check & accumulate:
        float rangeCheck= abs(-origin.z/u_nearFar.y - sampleDepth) < u_ssaoParameters[3] ? 1.0 : 0.0;
        //return rangeCheck;
        //return (sampleDepth <= zsample.z ? 1.0 : 0.0);
        //rangeCheck = 1.0;
        occlusion += (sampleDepth+0.01 < (-zsample.z/u_nearFar.y) ? 1.0 : 0.0) * rangeCheck;
    } 

    return 1.0 - (occlusion/float(numSamples));
}

void main() {
    FragColor = vec4(vec3(ssao()), 1.0);
    // FragColor = vec4(-v_texT2.zzz*0.005, 1.0);
}
