#version #opengl_version

#include "color_uniforms.glsl"

in vec3 tpos;
layout( location = 0 ) out vec4 FragColor;
layout( location = 1 ) out vec4 FragAttach1;

void main()
{
    float alphaV = 1.0;
    if ( tpos.z < 0.5 ) {
        vec2 p = tpos.xy;
        // Gradients
        vec2 px = dFdx(p);
        vec2 py = dFdy(p);
        // Chain rule
        float fx = ((2.0*p.x)*px.x - px.y);
        float fy = ((2.0*p.x)*py.x - py.y);
        // Signed distance
        float dist = fx*fx + fy*fy;
        float sd = (p.x*p.x - p.y)/sqrt(dist);
        // Linear alpha
        if ( sd > 0.5 ) {
            alphaV *= clamp(0.5 - sd, 0.0, 1.0);
        }
    } else {
        alphaV *= round((tpos.x * tpos.x - tpos.y) * tpos.z + 0.5f);
    }
    float aFactor = alpha*alphaV*opacity;
    FragColor = vec4(diffuseColor * aFactor, aFactor);
}
