
layout( std140 ) uniform CameraUniforms{
    mat4 u_mvpMatrix;
    mat4 u_viewMatrix;
    mat4 u_projMatrix;
    mat4 u_screenSpaceMatrix;
    vec3 u_eyePos;
    vec4 u_nearFar;            
};
    