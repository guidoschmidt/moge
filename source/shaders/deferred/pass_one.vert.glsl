#version 400

layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

uniform vec3 camera;
uniform mat4 mvp;

out vec3 vert_Position;
out vec3 vert_Normal;
out vec2 vert_UV;
out vec3 vert_Camera;

void main(void)
{	
	vert_Position = vec3(mvp * vec4(vertex, 1.0f));
	vert_Normal = normalize(normal);
	vert_UV = uv;
	gl_Position = mvp * vec4(vertex, 1.0f);
}