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
uniform sampler2D deferredZTex;
uniform sampler2D deferredDiffuseTex;
uniform sampler2D deferredDepthTex;


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
	float initalStep = 0.06f;
	float stepSize = 1.0f/Screen.Width;
	float blurSize = 1.0f;

	// Current fragment
	vec2 fragment = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
	vec2 ssfragment = 2.0f * fragment - 1.0f;
	vec3 ssPosition = vec3(ssfragment, 0.0f);
	ssPosition.z =  linearizeDepth( texture(deferredDepthTex, vert_UV) ); 

	// Normal & position
	vec3 vsNormal = normalize(texture(deferredNormalTex, fragment).xyz);
	vec3 vsPosition = texture(deferredPositionTex, fragment).xyz;
	
	// View vector
	vec3 vsViewVec = normalize( -ssPosition );
	//vsViewVec -= Camera.Position;

	// Reflection vector
	vec4 ReflectVec = texture(deferredReflecVecTex, fragment);
	vec3 vsReflectVec = normalize(ReflectVec ).xyz / ReflectVec.z;
	//vec3 vsReflectVec = reflect( vsViewVec, vsNormal);
	//vsReflectVec = normalize(vsReflectVec);
	vsReflectVec = vsReflectVec * initalStep;

	// Get depth informations
	float fragmentDepth = linearizeDepth( texture(deferredDepthTex, fragment) ); 
	vec2 samplingPosition = fragment + (vsReflectVec.xy);
	float sampledDepth = linearizeDepth( texture(deferredDepthTex, samplingPosition) );
	float rayDepth = fragmentDepth + vsReflectVec.z;

	// Ray tracing while in screen space
	int count = 0;
	
	/*
	while(	samplingPosition.x > 0.0f && samplingPosition.x < 1.0f &&
			samplingPosition.y > 0.0f && samplingPosition.y < 1.0f)
	{
		// Update sampling position and depth values
		samplingPosition = fragment + vsReflectVec.xy;
		sampledDepth = linearizeDepth( texture(deferredDepthTex, samplingPosition) );
		rayDepth = vsReflectVec.z;

		// Manually break
		if(count == 500)
			break;
		
		// intersection found
		if(rayDepth > sampledDepth)
		{
			//float delta = abs(rayDepth - sampledDepth);
			fragmentColor = vec4( texture(deferredDiffuseTex, samplingPosition).rgb, 1.0f );	
			fragmentColor = vec4(1.0f, 0.0f, 0.0f, 1.0f );	
			break;
		}
		
		//Jitter the initial ray
		if(jittering)
		{
			float randomOffset1 = clamp(rand(gl_FragCoord.yx), 0, 1)/(Screen.Width  * 15000f);
			float randomOffset2 = clamp(rand(gl_FragCoord.xy), 0, 1)/(Screen.Height * 15000f);
			float randomOffset3 = clamp(rand(gl_FragCoord.zz), 0, 1)/(fragmentDepth * 15000f);
			vsReflectVec += vec3(randomOffset1, randomOffset2, 0);
			stepSize += randomOffset3;
		}

		vsReflectVec += stepSize;
		count++;
	}
	*/

	if(mouseSlider)
	{
		// Variables for displaying 2 textures with divider on screen 
		float divider = Mouse.X/Screen.Width;
		float offset = 0.0009f;
		
		// Debugging depths
		if(compareDepth)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			// Right
			else if(fragment.x > (divider + offset))
				fragmentColor = vec4(sampledDepth, sampledDepth, sampledDepth, 1.0f);
			// Left
			else
				fragmentColor = vec4(rayDepth, rayDepth, rayDepth, 1.0f);	
		}
		// Debugging reflection vectors
		if(showReflVecs)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			// Right
			else if(fragment.x > (divider + offset))
				fragmentColor = texture(deferredReflectanceTex, samplingPosition);
			// Left
			else
				fragmentColor = vec4(vsReflectVec, 1.0f);	
		}
	}


	// Return color from sampled fragment
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
	// Z-Value
	else if(textureID == 6)
	{
		FragColor = texture(deferredZTex, vert_UV);
	}
	// Depth
	else if(textureID == 7)
		FragColor = vec4( linearizeDepth( float(texture(deferredDepthTex, vert_UV)) ) );
}