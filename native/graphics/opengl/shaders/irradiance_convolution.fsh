#version #opengl_version
#precision_high
out vec4 FragColor;
in vec3 v_texCoord;

uniform samplerCube cubeMapTexture;

const float PI = 3.14159265359;

void main()
{
    
    // The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(v_texCoord);

    vec3 irradiance = vec3(0.0);
    //irradiance += texture(cubeMapTexture, N).rgb;
    //FragColor = vec4(irradiance, 1.0);
    //return;
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0); 
    vec3 right = cross(up, N);
    up            = cross(N, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.50 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(cubeMapTexture, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples+=1.0;
        }
    }

    irradiance = PI * irradiance * (1.0 / nrSamples);

    FragColor = vec4(irradiance, 1.0);
    // FragColor = vec4( texture(cubeMapTexture, v_texCoord).xyz, 1.0);
    // FragColor = vec4( 1.0);
}
