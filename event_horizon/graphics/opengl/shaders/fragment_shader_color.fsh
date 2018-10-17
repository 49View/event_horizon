#version #opengl_version

uniform float alpha;
uniform float opacity;
uniform vec3  diffuseColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(diffuseColor, alpha*opacity);
}
    