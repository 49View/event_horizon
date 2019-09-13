#version #opengl_version

#include "color_uniforms.glsl"

in vec3 tpos;
out vec4 FragColor;

void main()
{
    float alphaV = 1.0;
    // alphaV = round((tpos.x * tpos.x - tpos.y) * tpos.z + 0.5f);
    if ( tpos.z < 0.0 ) {
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
        alphaV = clamp(0.5 - sd, 0.0, 1.0);
    }
    float aFactor = alpha*alphaV*opacity;
    FragColor = vec4(diffuseColor * aFactor, aFactor);
}
