#version #opengl_version

 varying float FragDepth;
// 
// varying vec4 FragDepth;

void main() {
    // FragDepth = gl_FragCoord.z;///gl_FragCoord.w;
    //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
    // FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
}
    