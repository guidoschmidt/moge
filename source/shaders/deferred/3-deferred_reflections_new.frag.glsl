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
uniform sampler2D deferredViewPositionTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredDiffuseTex;
uniform sampler2D deferredDepthTex;

// Variables for displaying 2 textures with divider on screen 
float divider = Mouse.X/Screen.Width;
float offset = 0.0009;

/*** Functions ****************************************************************/ 
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	return (2.0f * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}


/* Random float
 * @source: http://web.archive.org/web/20080211204527/http://lumina.sourceforge.net/Tutorials/Noise.html
 */
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


/* 
 * SSR (screen space reflections)
 * @date 	08.02.13
 * @author	Guido Schmidt
 */
vec4 SSR()
{
	vec4 shaded = vec4(0.0);

	float fragmentDepth = linearizeDepth( texture(deferredDepthTex, vert_UV).z );

	// Normal
	vec3 vsNormal = texture(deferredNormalTex, vert_UV).xyz;
	vec3 vsPosition = texture(deferredViewPositionTex, vert_UV).xyz;
	vsPosition.xy = (2.0f * (vert_UV) - 1.0f) * fragmentDepth;
	vsPosition.z = fragmentDepth;

	vec3 vsEyeVec = -normalize(vsPosition);
	vec4 vsReflectedVec = normalize( reflect( vec4(vsEyeVec, 0.0), vec4(vsNormal, 0.0) ) );

	vec3 ssTracedRay = (ProjectionMatrix * vsReflectedVec).xyz / vsReflectedVec.z;

	float min = 0.01;
	float max = 1.0;
	float delta = 0.01;

	ssTracedRay = min * ssTracedRay;
	
	float rayDepth = 0;
	float sampledDepth = linearizeDepth( texture(deferredDepthTex, ssTracedRay.xy).z );

	// Ray equation
	for(float t = min; t < max; t += delta)
	{


	}

	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{	
	// Reflection properties
	float reflectance = texture(deferredReflectanceTex, vert_UV).a;

	vec4 shaded = SSR();
	shaded.a = reflectance;
	
	if(toggleSSR)
		FragColor = shaded;
}