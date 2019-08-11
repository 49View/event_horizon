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

float vignetting() {
    // Vignetting
    float fstop = 0.4;
    float vignout = 1.0; // vignetting outer border
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

vec2 poissonDisk[8] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.44201624, -0.79906216 ),
    vec2( 0.24558609, -0.56890725 ),
    vec2( -0.694184101, -0.12938870 ),
    vec2( 0.84495938, 0.39387760 )
    );

float ssao() {
    float occluded = 1.0;
    float currZ = linearize(texture(depthMapTexture, v_texCoord).r);
    for ( int t = 0; t < 8; t++ ) {
        float sampledZ = linearize(texture(depthMapTexture, v_texCoord + poissonDisk[t]/200.0).r);
        if ( sampledZ < currZ && currZ - sampledZ < 0.1 ) {
            occluded -= 0.125;
        }
    }
    //if ( occluded > 0.5 ) occluded = 1.0;
    return occluded;
}

#include "dof.glsl"

void main() {

    vec4 sceneColor = texture(colorFBTexture, v_texCoord);
    // vec4 sceneColor = vec4(linearize( texture(depthMapTexture, v_texCoord).r ) / 16.0);
    // vec4 sceneColor = texture(shadowMapTexture, v_texCoord);

    // sceneColor.xyz = vec3(1.0) - exp(-sceneColor.xyz * u_hdrExposures.x*0.95);

    // vec3 origin = u_eyeDir.xyz * linearize( texture(depthMapTexture, v_texCoord).r );
    // sceneColor.xyz = origin;

    sceneColor.xyz = dof();
    sceneColor.xyz += texture(bloomTexture, v_texCoord).r*0.25;
    // sceneColor.xzy *= ssao();
    // sceneColor.xzy = texture(depthMapTexture, v_texCoord).rgb;
    // sceneColor.xyz = texture( lut3dTexture, sceneColor.xyz ).xyz;
    sceneColor.xyz *= vignetting();
    sceneColor.xyz *= grain();

    FragColor = sceneColor;
}
