#version 400

/*** Input ********************************************************************/
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

/*** Output *******************************************************************/
out vec2 vert_UV;
out vec3 vert_Position;
out vec3 vert_Normal;
out vec3 vert_EyePosition;

/*** Uniforms *****************************************************************/
uniform vec3 CameraPosition;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVPMatrix;

/*** Main *********************************************************************/
void main()
{
	vert_Position = ( ModelMatrix * vec4(vertex, 1.0f) ).xyz;
	vert_Normal = ( ModelMatrix * vec4(normal, 0.0f) ).xyz;
	vert_UV = uv;
	vert_EyePosition = CameraPosition;

	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}