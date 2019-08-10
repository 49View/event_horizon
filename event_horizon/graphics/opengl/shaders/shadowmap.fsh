#version #opengl_version

layout(location = 0) out float FragDepth;
//layout(location = 0) 
// out vec4 FragDepth;

void main() {
    //FragDepth = gl_FragCoord.z/gl_FragCoord.w;
    //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
    // FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
}
    