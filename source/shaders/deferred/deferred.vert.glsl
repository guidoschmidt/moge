#version 400

// INS
layout (location=0) in vec3 vertex;
layout (location=1) in vec2 uv;

// UNIFORMS
uniform vec3 camera;

//OUTS
out vec2 vert_UV;

// MAIN
void main(void)
{	
	vert_UV = uv;
	gl_Position = vec4(vertex, 1.0);
}