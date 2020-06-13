#version #opengl_version
#include "camera_uniforms.glsl"

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;
varying vec3 GNormal;
varying vec3 GPosition;
noperspective varying vec3 GEdgeDistance;
attribute vec3 v_norm[];
attribute vec3 Position_worldspace[];
void main()
{
 // Transform each vertex into viewport space
 vec3 p0 = vec3(u_projMatrix * (gl_in[0].gl_Position /
 gl_in[0].gl_Position.w));
 vec3 p1 = vec3(u_projMatrix * (gl_in[1].gl_Position /
 gl_in[1].gl_Position.w));
 vec3 p2 = vec3(u_projMatrix * (gl_in[2].gl_Position /
 gl_in[2].gl_Position.w));
 // Find the altitudes (ha, hb and hc)
 float a = length(p1 - p2);
 float b = length(p2 - p0);
 float c = length(p1 - p0);
 float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
 float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
 float ha = abs( c * sin( beta ) );
 float hb = abs( c * sin( alpha ) );
 float hc = abs( b * sin( alpha ) );
 // Send the triangle along with the edge distances
 GEdgeDistance = vec3( ha, 0, 0 );
 GNormal = v_norm[0];
 GPosition = Position_worldspace[0];
 gl_Position = gl_in[0].gl_Position;
 EmitVertex();
 GEdgeDistance = vec3( 0, hb, 0 );
 GNormal = v_norm[1];
 GPosition = Position_worldspace[1];
 gl_Position = gl_in[1].gl_Position;
 EmitVertex();
 GEdgeDistance = vec3( 0, 0, hc );
 GNormal = v_norm[2];
 GPosition = Position_worldspace[2];
 gl_Position = gl_in[2].gl_Position;
 EmitVertex();
 EndPrimitive();
}