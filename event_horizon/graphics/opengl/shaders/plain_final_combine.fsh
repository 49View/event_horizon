#version #opengl_version

#include "lighting_uniforms.glsl"
#include "camera_uniforms.glsl"

in vec2 v_texCoord;
out vec4 FragColor;
uniform sampler2D colorFBTexture;
uniform sampler2D bloomTexture;
uniform sampler2D shadowMapTexture;
uniform sampler3D lut3dTexture;
uniform sampler2D depthMapTexture;
uniform sampler2D normalMapTexture;
uniform sampler2D noise4x4Texture;

float vignetting() {
    // Vignetting
    float fstop = 0.9;
    float vignout = 1.3; // vignetting outer border
    float vignin = 0.0; // vignetting inner border
    float vignfade = 22.0; // f-stops till vignete fades
    float dist = distance(v_texCoord, vec2(0.5,0.5));
    dist = smoothstep(vignout+(fstop/vignfade), vignin+(fstop/vignfade), dist);
    return clamp(dist,0.0,1.0);
}

float grain() {
    float strength = 6.0;    
    float iTime = 1.748923;
    float x = (v_texCoord.x + 2.0 ) * (v_texCoord.y + 4.0 ) * (iTime * 10.0);
	return 1.0 - ((mod((mod(x, 17.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.02)-0.0075) * strength);
}

float linearize(float depth) {
    float znear = u_nearFar.x;
    float zfar = u_nearFar.y;
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

vec3 bloom( float amount ) {
    return vec3(texture(bloomTexture, v_texCoord).r*amount);
}

float ssao() {
    float occluded = 1.0;
    vec2 uNoiseScale = vec2(2560.0/4.0, 1440.0/4.0);
    vec3 origin = u_eyeDir.xyz * linearize( texture(depthMapTexture, v_texCoord).r );
    vec3 normal = texture(normalMapTexture, v_texCoord).xyz * 2.0 - 1.0;
    normal = normalize(normal);
    vec3 rvec = texture(noise4x4Texture, v_texCoord * uNoiseScale).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float currZ = linearize(texture(depthMapTexture, v_texCoord).r);
    // for ( int t = 0; t < u_hemisphereSampleKernelSize; t++ ) {
    //     float sampledZ = linearize(texture(depthMapTexture, v_texCoord + (u_hemisphereSampleKernel[t].xy/15.0)*rvec.xy).r);
    //     if ( sampledZ < currZ && currZ - sampledZ < 0.2) { //
    //         occluded -= 1.0/u_hemisphereSampleKernelSize;
    //     }
    // }

    float occlusion = 0.0;
    float uRadius = 0.5;///25.0;
    for (int i = 0; i < u_hemisphereSampleKernelSize; ++i) {
        // get sample position:
        vec3 zsample = tbn * u_hemisphereSampleKernel[i];
        // zsample = zsample * uRadius + origin;
        zsample = origin;
        
        // project zsample position:
        vec4 offset = vec4(zsample, 1.0);
        offset = u_projMatrix * offset;
        
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // get zsample depth:
        float sampleDepth = linearize(texture(depthMapTexture, offset.xy).r);
        vec3 sampleDepth3 = u_eyeDir.xyz * linearize(texture(depthMapTexture, offset.xy).r);
    return sampleDepth3.z;        
        // range check & accumulate:
        float rangeCheck= abs(origin.z - sampleDepth) < uRadius ? 1.0 : 0.0;
        rangeCheck = 1.0;
        occlusion += (sampleDepth <= zsample.z ? 1.0 : 0.0) * rangeCheck;
    }

    return 1.0 - (occlusion/u_hemisphereSampleKernelSize);
}

#include "dof.glsl"

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);
    // sceneColor.xyz = vec3(linearize( texture(depthMapTexture, v_texCoord).r ) / 4.0);
    // vec4 sceneColor = texture(shadowMapTexture, v_texCoord);

    // sceneColor.xyz = vec3(1.0) - exp(-sceneColor.xyz * u_hdrExposures.x*0.95);

    // sceneColor.xyz = origin;

    // sceneColor.xyz = dof();
    // sceneColor.xyz = vec3(1.0);

    // float currZ = linearize( texture(depthMapTexture, v_texCoord).r );
    // vec3 origin = u_eyeDir.xyz * linearize( texture(depthMapTexture, v_texCoord).r );

    // vec4 offset = vec4(origin, 1.0);
    //offset = u_projMatrix * offset;    
    // offset.xyz /= offset.w;
    // offset.xyz = offset.xyz * 0.5 + 0.5;
    
    // vec3 origin2 = u_eyeDir.xyz * linearize(texture(depthMapTexture, offset.xy).r);
    // float origin2Z = linearize(texture(depthMapTexture, offset.xy).r);
    // float z2 = linearize(texture(depthMapTexture, offset.xy).r);
    // sceneColor.xyz = vec3(z2);
    // sceneColor.xyz = vec3(origin2.z);
    // sceneColor.xyz = vec3(offset.xyz);
    //sceneColor.xyz = vec3(origin.z/10);
    // sceneColor.xyz = vec3(currZ/10);
    // sceneColor.xyz = vec3(origin);
    // sceneColor.xyz = vec3(abs(currZ-origin.z));
    //sceneColor.xyz = vec3(abs(origin2Z)/10);
    // sceneColor.xyz = vec3(abs(v_texCoord-offset.xy), 0.0);
    // sceneColor.xyz = vec3(v_texCoord, 0.0);
    // sceneColor.xyz = vec3((offset.xyz)/10);

    // sceneColor.xyz = texture(normalMapTexture, v_texCoord).rgb;

    // sceneColor.xyz *= ssao();
    // sceneColor.xyz = texture( lut3dTexture, sceneColor.xyz ).xyz;
    sceneColor.xyz += bloom(0.35); 
    sceneColor.xyz *= vignetting();
    sceneColor.xyz *= grain();

    FragColor = sceneColor;
}
