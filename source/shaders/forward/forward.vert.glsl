#version 400

/*** Input ********************************************************************/
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

/*** Output *******************************************************************/
out vec4 vert_Position;
out vec4 vert_Normal;
out vec2 vert_UV;

/*** Uniforms *****************************************************************/
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

/*** Main *********************************************************************/
void main(void)
{	
	vert_Position = ModelMatrix * vec4(vertex, 1.0f);
	vert_Normal = normalize(NormalMatrix * vec4(normal, 0.0f));
	vert_UV = uv;
	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}