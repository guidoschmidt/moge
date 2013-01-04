#version 400

layout (location=0) in vec3 vertex;
layout (location=1) in vec2 uv;

uniform vec3 camera;

out vec2 vert_UV;

void main(void)
{	
	vert_UV = uv;
	gl_Position = vec4(vertex, 1.0);
}