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
out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform MouseInfo Mouse;

uniform int textureID;

uniform bool toggleSSR;
uniform bool blur;
uniform bool compareDepth;
uniform bool showReflVecs;
uniform bool jittering;
uniform bool mouseSlider;

uniform float deltaDepth;
	
uniform	mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
	
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredReflecVecTex;
uniform sampler2D deferredDepthTex;
uniform sampler2D deferredDiffuseTex;


/*** Functions ****************************************************************/ 
float linearizeDepth(float depth)
{
	return (2.0 * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

// Random float
/*
 * @source: http://web.archive.org/web/20080211204527/http://lumina.sourceforge.net/Tutorials/Noise.html
 */
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/******************************************************************************/
// LATEST (working, with some perspective errors)
/* SSR (screen space reflections)
 * @date 	27.01.13
 * @author	Guido Schmidt
 ******************************************************************************/
vec4 SSR()
{
	// Variables
	vec4 fragmentColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// Current fragment
	vec2 fragment = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
	// Size of one fragment
	vec2 fragmentSize = 1.0f/vec2(Screen.Width, Screen.Height);

	// Normals from g-Buffer
	vec4 vsNormal = texture(deferredNormalTex, fragment);

	// Position from g-Buffer
	vec4 vsPosition = texture(deferredPositionTex, fragment);

	// Screen space position
	vec3 ssPosition = vec3(2.0f * fragment.xy - 1.0f, 0.0f);
	ssPosition.z = 1 - linearizeDepth( texture(deferredDepthTex, fragment).z );

	// Reflection vector
	vec4 vsRevlectionVec = normalize( texture(deferredReflecVecTex, fragment) );
	vec3 ssReflectionVec = ( ProjectionMatrix * vsRevlectionVec ).xyz / vsRevlectionVec.w;
	ssReflectionVec = 0.5f * ssReflectionVec + 0.5f;

	// Sampling positions
	vec3 lastPosition = ssPosition + ssReflectionVec;
	vec3 currPosition = lastPosition + ssReflectionVec;
	float currDepth, sampledDepth;

	// Raytracing
	int count = 0;
	while(count < 100)
	{
		vec2 samplePos2D = (0.5f * currPosition.xy + 0.5f);
		float currDepth = linearizeDepth(currPosition.z);
		float sampledDepth = linearizeDepth( texture(deferredDepthTex, samplePos2D).z );

		if(currDepth > sampledDepth)
		{
			fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		// Step ray
		lastPosition = currPosition;
		currPosition = lastPosition + ssReflectionVec;

		count++;
	}

	fragmentColor = texture(deferredDiffuseTex, fragment);

	// Variables for displaying 2 textures with divider on screen
	if(mouseSlider)
	{
		float divider = Mouse.X/Screen.Width;
		float offset = 0.0009f;
		
		// Debugging depths
		if(compareDepth)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			else if(fragment.x > (divider + offset))
				fragmentColor = vec4(currDepth, currDepth, currDepth, 1.0f);
			else
				fragmentColor = vec4(lastPosition, 1.0f);	
		}
		// Debugging reflection vectors
		if(showReflVecs)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			else if(fragment.x > (divider + offset))
				fragmentColor = vsNormal;
			else
				fragmentColor = vsRevlectionVec;	
		}
	} 

	return fragmentColor;
}

/*** Main *********************************************************************/
void main(void)
{	
	// Compositing
	if(textureID == -1)
	{
		// Reflection properties
		float reflectance = texture(deferredReflectanceTex, vert_UV).a;
		// Cubemap shading
		//vec3 cubemapColor = texture(deferredReflectanceTex, vert_UV).rgb;
		// Diffuse shading
		vec4 shaded = texture(deferredDiffuseTex, vert_UV);
		
		// Reflections
		FragColor = shaded;
		if(toggleSSR) // Screen space reflections
		{
			if(reflectance > 0.0f)
			{
				FragColor = reflectance * SSR() + (1.0f - reflectance) * shaded;
			}
		}
		if(mouseSlider)
			FragColor = SSR();
	}
	// Positions
	else if(textureID == 0)
		FragColor = texture(deferredPositionTex, vert_UV);
	// Albedo (Color)
	else if(textureID == 1)
		FragColor = texture(deferredColorTex, vert_UV);
	// Normals
	else if(textureID == 2)
		FragColor = texture(deferredNormalTex, vert_UV);
	// Material IDs
	else if(textureID == 3)
		FragColor = texture(deferredMaterialIDTex, vert_UV);
	// Reflectance
	else if(textureID == 4)
	{
		FragColor = vec4( texture(deferredReflectanceTex, vert_UV).a );
	}
	// Reflection vector
	else if(textureID == 5)
	{
		FragColor = texture(deferredReflecVecTex, vert_UV);
	}
	// Depth
	else if(textureID == 6)
		FragColor = vec4( linearizeDepth( float(texture(deferredDepthTex, vert_UV)) ) );
}