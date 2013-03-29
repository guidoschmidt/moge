//VERTEX SHADER
#version 400

//*** Uniform block definitions ************************************************
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

//*** Input ********************************************************************
layout (location=0) in vec3 vertex;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

//*** Output *******************************************************************
out vec3 vert_wsPosition;
out vec3 vert_wsNormal;
out vec3 vert_vsPosition;
out vec3 vert_vsNormal;
out vec2 vert_UV;
out vec3 vert_vsEyeVector;
out vec3 vert_wsEyePosition;
out vec3 vert_wsEyeVector;

//*** Uniforms *****************************************************************
uniform CameraInfo Camera;

uniform mat4 wsNormalMatrix;
uniform mat4 vsNormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

//*** Main *********************************************************************
void main(void)
{	
	//*** Positions & normals ***
	// World space
	vec3 wsPosition = ( ModelMatrix * vec4(vertex, 1.0f) ).xyz;
	vert_wsPosition = wsPosition; 
	vert_wsNormal = ( wsNormalMatrix * vec4(normal, 0.0f) ).xyz;
	// View space
	vec3 vsPosition = ( ViewMatrix * (ModelMatrix * vec4(vertex, 1.0f)) ).xyz;
	vert_vsPosition = vsPosition;
	vert_vsNormal = ( vsNormalMatrix * vec4(normal, 0.0f) ).xyz;
	
	//*** Texture coordinates ***
	vert_UV = uv;
	
	//*** Eye vectors ***
	vert_vsEyeVector = vert_vsPosition;
	vert_wsEyePosition = Camera.Position;
	vert_wsEyeVector = vert_wsPosition - Camera.Position;

	//*** Transform vertex with Model-View-Projection-Matrix ***
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex, 1.0f);
}