#version #opengl_version

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"

varying vec2 v_texCoord;
varying vec3 v_texT2;
uniform sampler2D depthMapTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D noise4x4Texture;

float ssao() {
    float occluded = 1.0;
    vec2 uNoiseScale = vec2(1024.0/4.0, 1024.0/4.0);
    float pointZ = texture2D(depthMapTexture, v_texCoord).r;
    
    vec3 origin = -v_texT2 * pointZ;

    vec3 normal = texture2D(normalMapTexture, v_texCoord).xyz * 2.0 - 1.0;
    normal = normalize(normal);
    normal.y = -normal.y;
    vec3 rvec = texture2D(noise4x4Texture, v_texCoord * uNoiseScale).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, -normal);

    float occlusion = 0.0;
    float pRangeClamped = clamp(0.0, 1.0, pointZ);
    float uRadius = u_ssaoParameters[0]*pRangeClamped; // 10.0 default;
    float uRadius100 = uRadius*0.01;
    // ###WEBGL1###
    // hardcode numSamples
    const int numSamples = 4;//int(u_ssaoParameters[1]); // u_hemisphereSampleKernelSize
    for (int i = 0; i < numSamples; ++i) {
        // get sample position:
        vec3 sam = u_hemisphereSampleKernel[i];
        vec3 zsample = tbn * sam;
        zsample = zsample * uRadius + origin;
        float compZ = zsample.z*0.01;

        // project zsample position:
        vec4 offset = vec4(zsample, 1.0);
        offset = u_projMatrix * offset;        
        offset.xy /= -offset.w;
        offset.xy = offset.xy * vec2(-0.5, 0.5) + 0.5;
        
        // get zsample depth:
        float sampleDepth = texture2D(depthMapTexture, offset.xy).r;

        float rangeCheck = smoothstep(0.0, 1.0, (uRadius100)/abs(pointZ-sampleDepth));

        occlusion += (sampleDepth > (compZ-0.0125) ? 0.0 : 1.0) * rangeCheck;
    } 

    return 1.0-(occlusion/float(numSamples));
}

void main() {
    gl_FragColor = vec4(vec3(ssao()), 1.0);
}
