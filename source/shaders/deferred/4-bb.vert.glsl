#version 400

layout (location = 0) in vec3 vertex;

uniform mat4 MVPMatrix;

void main()
{
	gl_Position = vec4(vertex, 1.0);
}