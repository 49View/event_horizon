#version #opengl_version

varying vec3 v_norm;

// 
// varying vec4 FragDepth;

void main() {
    gl_FragColor = vec4(v_norm * 0.5 + 0.5, 1.0);// ;
    // FragDepth = gl_FragCoord.z;///gl_FragCoord.w;
    //FragDepth = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);//gl_FragCoord.z;
    // FragDepth = vec4(0.5, 0.5, 0.5, 1.0);//gl_FragCoord.z;
}
