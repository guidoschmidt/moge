#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

struct MouseInfo
{
	float X;
	float Y;
};

struct ScreenInfo
{
	float Width;
	float Height;
};

/*** Input ********************************************************************/
in vec2 vert_UV;

/*** Output *******************************************************************/
layout (location = 0) out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform MouseInfo Mouse;

uniform float rayStepSize;

uniform bool toggleSSR;
uniform bool compareDepth;
uniform bool showReflVecs;
uniform bool jittering;
uniform bool mouseSlider;
	
uniform	mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
	
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredReflectionVecTex;
uniform sampler2D deferredDiffuseTex;
uniform sampler2D deferredDepthTex;

// Variables for displaying 2 textures with divider on screen 
float divider = Mouse.X/Screen.Width;
float offset = 0.0009f;

/*** Functions ****************************************************************/ 
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	return (2.0f * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

/*** Main *********************************************************************/
void main(void)
{	
	vec4 shaded = vec4(0.0f);


	FragColor = shaded;
}