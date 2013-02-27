#version 400

/*** Input ********************************************************************/
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;


/*** Output *******************************************************************/
out vec3 vert_wsPosition;
out vec3 vert_vsPosition;
out vec3 vert_vsNormal;
out vec3 vert_wsNormal;
out vec2 vert_UV;
out vec3 vert_vsEyeVector;
out vec3 vert_wsEyeVector;

/*** Uniforms *****************************************************************/
uniform vec3 CameraPosition;
uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

/*** Main *********************************************************************/
void main(void)
{	
	vert_wsPosition = ( ModelMatrix * vec4(vertex, 1.0f) ).xyz;
	vert_vsPosition = ( ViewMatrix * ModelMatrix * vec4(vertex, 1.0f) ).xyz;
	vert_vsNormal = ( NormalMatrix * vec4(normal, 0.0f) ).xyz;
	vert_wsNormal = ( transpose(inverse(ModelMatrix)) * vec4(normal, 0.0f) ).xyz;
	
	vert_UV = uv;
	
	vert_vsEyeVector = -vert_vsPosition;
	vert_wsEyeVector = -vert_wsPosition;

	gl_Position = MVPMatrix * vec4(vertex, 1.0f);
}