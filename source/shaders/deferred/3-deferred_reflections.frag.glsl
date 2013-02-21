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


/* Random float
 * @source: http://web.archive.org/web/20080211204527/http://lumina.sourceforge.net/Tutorials/Noise.html
 */
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/*
 * Raytracing to get reflected color
 */
vec3 raytrace(in vec3 reflectionVector, in float startDepth)
{
	vec3 color = vec3(0.0f);
	float stepSize = rayStepSize; 

	float size = length(reflectionVector.xy);
	reflectionVector = normalize(reflectionVector/size);
	reflectionVector = reflectionVector * stepSize;

	vec2 sampledPosition = vert_UV;
	float currentDepth = startDepth - reflectionVector.z;
	float sampledDepth = linearizeDepth( texture(deferredDepthTex, sampledPosition).z );

	vec2 jitter = vec2(0.0f);
	if(jittering)
	{
		jitter.x = rand( (vert_UV.xy - 0.5f) * gl_FragCoord.xy) / (20.0f * Screen.Width);
		jitter.y = rand( (vert_UV.yx - 0.5f) * gl_FragCoord.yx) / (20.0f * Screen.Height);
	}

	int count = 0;
	while(sampledPosition.x <= 1.0 && sampledPosition.x >= 0.0 &&
	      sampledPosition.y <= 1.0 && sampledPosition.y >= 0.0)
	{
		if(jittering)
		{
			sampledPosition.x = sampledPosition.x + reflectionVector.x + jitter.x;
			sampledPosition.y = sampledPosition.y + reflectionVector.y + jitter.y;
		}
		else
			sampledPosition = sampledPosition + reflectionVector.xy;
		
		currentDepth = currentDepth + reflectionVector.z * startDepth;
		float sampledDepth = linearizeDepth( texture(deferredDepthTex, sampledPosition).z );

		if(currentDepth > sampledDepth)
		{
			float delta = (currentDepth - sampledDepth);
			if(delta < 0.003f )
			{
				color = texture(deferredDiffuseTex, sampledPosition).rgb;
				break;
			}
		}

		//! Manual break, better performance
		if(count == 200)
			break;
		
		count++;
	}

	// Debugging depths
	if(mouseSlider && compareDepth)
	{
		if((vert_UV.x <= (divider + offset)) && (vert_UV.x >= (divider - offset)))
			color = vec3(1.0f, 0.0f, 0.0f);
		// Right side
		else if(vert_UV.x > (divider + offset))
			color = vec3(sampledDepth);
		// Left side
		else
			color = vec3(currentDepth);	
	}

	return color;
}

/* 
 * SSR (screen space reflections)
 * @date 	08.02.13
 * @author	Guido Schmidt
 */
vec4 SSR()
{
	vec3 reflectedColor = vec3(0.0f);

	vec3 normal = normalize( texture(deferredNormalTex, vert_UV) ).xyz;
	vec3 position = texture(deferredPositionTex, vert_UV).xyz;
	/*
	vec3 position = vec3(gl_FragCoord.xy/vec2(Screen.Width, Screen.Height), 0.0f);
	position.z = texture(deferredDepthTex, vert_UV).z;
	position = (vec4(position, 1.0f) * transpose(ProjectionMatrix) ).xyz;
	position = 0.5f * position + 1.0f;
	*/

	// Depth at current fragment
	float currDepth = linearizeDepth( texture(deferredDepthTex, vert_UV).z ) ;

	// Eye position, camera is at (0, 0, 0), we look along negative z, add near plane to correct parallax
	vec3 eyePosition = vec3(position.x, position.y, Camera.FarPlane);
	vec4 reflectionVector = normalize( reflect( vec4(eyePosition, 0), vec4(normal, 0) ) );
	reflectionVector = ProjectionMatrix * reflectionVector;
	reflectionVector.xyz = reflectionVector.xyz / reflectionVector.w;

	// Debugging reflection vectors
	if(mouseSlider && showReflVecs)
	{
		if((vert_UV.x <= (divider + offset)) && (vert_UV.x >= (divider - offset)))
			reflectedColor = vec3(1.0f, 0.0f, 0.0f);
		// Right side
		else if(vert_UV.x > (divider + offset))
			reflectedColor = normal;
		// Left side
		else
			reflectedColor = reflectionVector.xyz;	
	}
	// No debugging
	else
	{
		reflectedColor = raytrace(reflectionVector.xyz, currDepth);	
	}

	return vec4(reflectedColor, 1.0f);
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