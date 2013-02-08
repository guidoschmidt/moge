#version 400

layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

out vec3 vert_Position;
out vec3 vert_Normal;
out vec2 vert_UV;

uniform vec3 CameraPosition;
uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

void main()
{
	vert_Position = vec3( ViewMatrix * ModelMatrix * vec4(vertex, 1.0f) );
	vert_Normal = vec3( NormalMatrix * vec4(normal, 0.0f) );
	vert_UV = uv;

	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}