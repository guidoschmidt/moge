#version 400

// INS
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

// UNIFORMS
uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

// OUTS
out vec3 vert_Position;
out vec3 vert_Normal;
out vec2 vert_UV;
out vec3 vert_Camera;

// MAIN
void main(void)
{	
	vert_Position = vec3(ModelMatrix * vec4(vertex, 1.0f));
	// Normals are multiplied with transposed inverse model view matrix
	vert_Normal = normalize(vec3(NormalMatrix * vec4(normal, 0.0f)));
	vert_UV = uv;
	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}