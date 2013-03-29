//FRAGMENT SHADER
#version 400 core

//*** Uniform block definitions ************************************************
// Screen/Viewport informations
struct ScreenInfo
{
	float Width;
	float Height;
};

// Camera informations
struct CameraInfo
{
	float FOV;
	float NearPlane;
	float FarPlane;
	vec3 LookAt;
	vec3 Position;
};

//*** Input ********************************************************************
in vec2 vert_UV;

//*** Output *******************************************************************
out vec4 FragColor;

//*** Uniforms *****************************************************************
uniform ScreenInfo Screen;
uniform CameraInfo Camera;

uniform int user_pixelStepSize;
uniform bool toggleSSR;
uniform bool optimizedSSR;
uniform bool fadeToEdges;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D wsPositionTex;
uniform sampler2D vsPositionTex;
uniform sampler2D wsNormalTex;
uniform sampler2D vsNormalTex;
uniform sampler2D ReflectanceTex;
uniform sampler2D DepthTex;
uniform sampler2D DiffuseTex;

const float pi = 3.14159265f;

//*** Functions **************************************************************** 
//	Linearizes a depth value
//	Source:	http://www.geeks3d.com/20091216/geexlab-how-to-visualize-the-depth-buffer-in-glsl/
float linearizeDepth(float depth)
{
	float near = Camera.NearPlane;
	float far = Camera.FarPlane;
	float linearDepth = (2.0 * near) / (far + near - depth * (far - near));

	return linearDepth;
}

//	Screen space reflections
vec4 ScreenSpaceReflections(in vec3 vsPosition, in vec3 vsNormal, in vec3 vsReflectionVector)
{
	// Variables
	vec4 reflectedColor = vec4(0.0);
	vec2 pixelsize = 1.0/vec2(Screen.Width, Screen.Height);

	// Get texture informations
	vec4 csPosition = ProjectionMatrix * vec4(vsPosition, 1.0);
	vec3 ndcsPosition = csPosition.xyz / csPosition.w;
	vec3 ssPosition = 0.5 * ndcsPosition + 0.5;

	// Project reflected vector into screen space
	vsReflectionVector += vsPosition;
	vec4 csReflectionVector = ProjectionMatrix * vec4(vsReflectionVector, 1.0);
	vec3 ndcsReflectionVector = csReflectionVector.xyz / csReflectionVector.w;
	vec3 ssReflectionVector = 0.5 * ndcsReflectionVector + 0.5;
	ssReflectionVector = normalize(ssReflectionVector - ssPosition);

	// Ray trace
	float initalStep = max(pixelsize.x, pixelsize.y);
	float pixelStepSize = user_pixelStepSize;
	ssReflectionVector *= initalStep * pixelStepSize;

	vec3 lastSamplePosition = ssPosition + ssReflectionVector;
	vec3 currentSamplePosition = lastSamplePosition + ssReflectionVector;

	int sampleCount = max(int(Screen.Width), int(Screen.Height));
	int count = 0;
	float refinementStep = pixelStepSize/100.0;
	int refinementCount = 0;
	int maxRefinements = 3;

	if(optimizedSSR)
	{
		while(count < sampleCount)
		{
			// Out of screen space --> break
			if(currentSamplePosition.x < 0.0 || currentSamplePosition.x > 1.0 ||
			   currentSamplePosition.y < 0.0 || currentSamplePosition.y > 1.0 ||
			   currentSamplePosition.z < 0.0 || currentSamplePosition.z > 1.0)
			{
				break;
			}
			
			vec2 samplingPosition = currentSamplePosition.xy;
			float currentDepth = linearizeDepth(currentSamplePosition.z);
			float sampledDepth = linearizeDepth( texture(DepthTex, samplingPosition).z );

			if(currentDepth > sampledDepth)
			{	
				
				
				ssReflectionVector *= refinementStep;
				currentSamplePosition = lastSamplePosition;
				refinementCount++;
				if(refinementCount > maxRefinements)
				{
					break;
				}

				float delta = abs(currentDepth - sampledDepth);
				if(delta < 0.001f)
				{
					reflectedColor.rgb = texture(DiffuseTex, samplingPosition).rgb; 
				}
				//break;	
			}

			// Step ray
			lastSamplePosition = currentSamplePosition;
			currentSamplePosition = lastSamplePosition + ssReflectionVector;	
			
			
			count++;
		}
	}
	//*** Unoptimized approach ***
	else
	{
		while(count < sampleCount)
		{
			// Out of screen space --> break
			if(currentSamplePosition.x < 0.0 || currentSamplePosition.x > 1.0 ||
			   currentSamplePosition.y < 0.0 || currentSamplePosition.y > 1.0 ||
			   currentSamplePosition.z < 0.0 || currentSamplePosition.z > 1.0)
			{
				break;
			}
			
			vec2 samplingPosition = currentSamplePosition.xy;
			float currentDepth = linearizeDepth(currentSamplePosition.z);
			float sampledDepth = linearizeDepth( texture(DepthTex, samplingPosition).z );

			if(currentDepth > sampledDepth)
			{	
				
				float delta = abs(currentDepth - sampledDepth);
				if(delta <= 0.001f)
				{
					reflectedColor = texture(DiffuseTex, samplingPosition); 
					break;	
				}
			}

			// Step ray
			lastSamplePosition = currentSamplePosition;
			currentSamplePosition = lastSamplePosition + ssReflectionVector;	
			
			count++;
		}
	}

	// Fading to screen edges
	vec2 fadeToScreenEdge = vec2(1.0);
	if(fadeToEdges)
	{
		fadeToScreenEdge.x = distance(lastSamplePosition.x , 1.0);
		fadeToScreenEdge.x *= distance(lastSamplePosition.x , 0.0) * 4.0;
		fadeToScreenEdge.y = distance(lastSamplePosition.y , 1.0);
		fadeToScreenEdge.y *= distance(lastSamplePosition.y , 0.0) * 4.0;
	}

	return reflectedColor * fadeToScreenEdge.x * fadeToScreenEdge.y;
}

//*** Main *********************************************************************
void main(void)
{
	//*** Texture information from G-Buffer ***
	float reflectance = texture(ReflectanceTex, vert_UV).a;
	vec3 vsPosition         = texture(vsPositionTex, vert_UV).xyz;
	vec3 vsNormal           = texture(vsNormalTex, vert_UV).xyz;

	//*** Reflection vector calculation ***
	// View space calculations
	vec3 vsEyeVector        = normalize(vsPosition);
	vec3 vsReflectionVector = normalize(reflect(vsEyeVector, vsNormal));			

	//*** Screen space reflections ***
	if(toggleSSR)
	{
		vec4 color = ScreenSpaceReflections(vsPosition, vsNormal, vsReflectionVector);
		//color = mix(color, texture(ReflectanceTex, vert_UV), 0.5);
		FragColor = reflectance * color;
	}
}