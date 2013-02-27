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

uniform samplerCube cubeMapTex;

// Variables for displaying 2 textures with divider on screen 
float divider = Mouse.X/Screen.Width;
float offset = 0.0009;

/*** Functions ****************************************************************/ 
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	return (Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
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
vec3 raytrace(in vec4 position, in vec3 reflectionVector, in float startDepth)
{
	vec3 color = vec3(0.0);
	vec2 pixelSize = 1.0/vec2(Screen.Width, Screen.Height);
	float stepSize = pixelSize.x;//rayStepSize/10.0;
	float initalStep = 0.001;

	reflectionVector = (initalStep * reflectionVector) / startDepth;

	vec2 sampledPosition = vert_UV;
	float currentDepth   = startDepth - reflectionVector.z;
	float sampledDepth   = linearizeDepth( texture(deferredDepthTex, sampledPosition).z );

	vec2 jitter = vec2(0.0);
	if(jittering)
	{
		jitter.x = rand( (vert_UV.xy - 0.5) * gl_FragCoord.xy) / (20.0 * Screen.Width);
		jitter.y = rand( (vert_UV.yx - 0.5) * gl_FragCoord.yx) / (20.0 * Screen.Height);
	}

	int count = 0;
	while(reflectionVector.x <= 1.0 && reflectionVector.x >= -1.0 &&
	      reflectionVector.y <= 1.0 && reflectionVector.y >= -1.0 && 
	      reflectionVector.z <= 1.0 && reflectionVector.z >= -1.0)
	{
		reflectionVector += (stepSize * reflectionVector) / startDepth;
		
		if(jittering)
		{
			sampledPosition.x = sampledPosition.x + reflectionVector.x + jitter.x;
			sampledPosition.y = sampledPosition.y + reflectionVector.y + jitter.y;
		}
		else
			sampledPosition = sampledPosition + reflectionVector.xy;
		
		currentDepth = currentDepth + (reflectionVector.z * startDepth);
		float sampledDepth = linearizeDepth( texture(deferredDepthTex, sampledPosition).z );

		if(currentDepth >= sampledDepth)
		{
			float delta = (currentDepth - sampledDepth);
			if(delta <  0.002)
			{
				color = texture(deferredDiffuseTex, sampledPosition).rgb;
				break;
			}
		}
		else
			color = texture(deferredReflectanceTex, sampledPosition).rgb;

		//! Manual break, better performance
		if(count == 200)
			break;
		
		count++;
	}

	// Debugging depths
	if(mouseSlider && compareDepth)
	{
		if((vert_UV.x <= (divider + offset)) && (vert_UV.x >= (divider - offset)))
			color = vec3(1.0, 0.0, 0.0);
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
	vec3 reflectedColor = vec3(0.0);

	// Depth at current fragment
	float currDepth = linearizeDepth( texture(deferredDepthTex, vert_UV).z ) ;

	vec4 normal = normalize( texture(deferredNormalTex, vert_UV) );
	vec4 position = texture(deferredViewPositionTex, vert_UV);
	
	position.xy = (2.0 * vert_UV - 1.0) * currDepth;
	position.z = (1.0 - currDepth) / Camera.NearPlane;


	vec3 ssPosition = ( ProjectionMatrix * position ).xyz / position.w;

	// Eye position, camera is at (0, 0, 0), we look along negative z, add near plane to correct parallax
	vec3 eyePosition = position.xyz;
	vec4 reflectionVector = normalize( reflect( vec4(eyePosition, 0), normal ) );
	reflectionVector = ProjectionMatrix * reflectionVector;
	reflectionVector.xyz = reflectionVector.xyz / reflectionVector.w;

	// Debugging reflection vectors
	if(mouseSlider && showReflVecs)
	{
		if((vert_UV.x <= (divider + offset)) && (vert_UV.x >= (divider - offset)))
			reflectedColor = vec3(1.0, 0.0, 0.0);
		// Right side
		else if(vert_UV.x > (divider + offset))
			reflectedColor = normal.xyz;
		// Left side
		else
			reflectedColor = reflectionVector.xyz;	
	}
	// No debugging
	else
	{
		reflectedColor = raytrace(position, reflectionVector.xyz, currDepth);	
	}

	return vec4(reflectedColor, 1.0);
}

/*** Main *********************************************************************/
void main(void)
{	
	// Reflection properties
	float reflectance = texture(deferredReflectanceTex, vert_UV).a;

	vec4 shaded = SSR();
	shaded.a = reflectance;
	
	if(toggleSSR)
		FragColor = reflectance * shaded;
}