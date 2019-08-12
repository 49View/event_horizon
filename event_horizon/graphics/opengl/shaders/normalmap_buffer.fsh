#version #opengl_version

layout(location = 0) out vec4 FragNormal;

in vec3 v_norm;

//layout(location = 0) 
// out vec4 FragDepth;

void main() {
    FragNormal = vec4(v_norm * 0.5 + 0.5, 1.0);// ;
    // FragDepth = gl_FragCoord.z;///gl_FragCoord.w;
    //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
    // FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
}
