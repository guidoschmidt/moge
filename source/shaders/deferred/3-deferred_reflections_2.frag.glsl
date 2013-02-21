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
	vec4 color = vec4(0.0f);
	float stepSize = 0.0f;

	vec3 position = texture(deferredPositionTex, vert_UV).xyz;
	position.z = linearizeDepth( texture(deferredDepthTex, vert_UV).z );;

	vec3 normal = texture(deferredNormalTex, vert_UV).xyz;

	vec4 reflectionVec = texture( deferredReflectionVecTex,vert_UV);
	reflectionVec *= inverse(ProjectionMatrix);
	reflectionVec.z += Camera.NearPlane;

	// Ray equation: R = O + d * D
	vec3 tracedRay = position + (stepSize * reflectionVec.xyz);

	//
	vec2 samplePosition2D = vert_UV +  reflectionVec.xy;
	float sampleDepth = linearizeDepth( texture(deferredDepthTex, samplePosition2D).z ); 
	float rayDepth = tracedRay.z; 

	vec3 rayStep = reflectionVec.xyz * stepSize;
	vec2 rayStep2D = ((ProjectionMatrix) * vec4(rayStep, 1.0f)).xy;

	float reflectance = texture(deferredReflectanceTex, vert_UV).a;

	if(reflectance < 0.0f)
	{
		int count = 0;
		while(count < 500)
		{
			rayStep += reflectionVec.xyz * stepSize;
			rayStep2D = (ProjectionMatrix * vec4(rayStep, 0.0f)).xy;
			tracedRay = position + rayStep.xyz;
			samplePosition2D = vert_UV + rayStep2D.xy;

			sampleDepth = linearizeDepth( texture(deferredDepthTex, samplePosition2D).z );
			rayDepth = tracedRay.z;

			if(rayDepth > sampleDepth)
			{
				float delta = abs(rayDepth - sampleDepth);
				if(delta < 0.005f)
					color = texture(deferredDiffuseTex, samplePosition2D, 5);					
			}

			stepSize += 0.00025f;
			count++;
		}
	}

	FragColor = color;


}