#version #opengl_version

#include "color_uniforms.glsl"

out vec4 FragColor;

void main()
{
// NDDado: this is a barycentric wireframe shader, needs a full vertex
// descriptor with barycentric coordinates IE v_t8 for SH shaders.

// float f_thickness = 0.001;
// float f_closest_edge = min(v_t8.x,min(v_t8.y, v_t8.z)); // see to which edge this pixel is the closest
// float f_width = fwidth(f_closest_edge); // calculate derivative (divide f_thickness by this to have the line width constant in screen-space)
// float f_alpha = smoothstep(f_thickness, f_thickness + f_width, f_closest_edge); // calculate alpha
// // opacityV *= 1.0-f_alpha;

    FragColor = vec4(diffuseColor, alpha*opacity);
}
 