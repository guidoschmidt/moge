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
uniform float rayMultiplier;
	
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
 ******************************************************************************/
vec4 SSR()
{
	// Variables
	vec4 fragmentColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	float initalStep = 0.001f;
	float stepSize = 0.1f;
	float blurSize = 1.0f;
	float refineStep = 0.5f;

	// Current fragment
	vec2 fragment = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
	vec2 ssfragment = 2.0f * fragment - 1.0f;
	vec3 ssPosition = vec3(ssfragment, 0.0f);
	ssPosition.z = linearizeDepth( texture(deferredDepthTex, vert_UV) ); 
	
	vec3 tsPosition = (0.5f * ssPosition) + 0.5f;

	// Normals and positions
	vec3 vsNormal = normalize(texture(deferredNormalTex, fragment).xyz);
	vec3 vsPosition = texture(deferredPositionTex, fragment).xyz;
	
	// View vector
	vec3 vsViewVector = normalize(-ssPosition);

	// Reflection vectors
	//vec4 vsReflectVec = reflect(vec4(vsViewVector, 0.0f), vec4(vsNormal, 0.0f));
	vec4 vsReflectVec = texture(deferredReflecVecTex, fragment);
	vsReflectVec = normalize(vsReflectVec);
	//vsReflectVec.xyz += vsPosition;
	//vsReflectVec *= initalStep;
	vec3 ssReflectVec = ( ProjectionMatrix * vsReflectVec ).xyz / vsReflectVec.z;
	//ssReflectVec += ssPosition;
	ssReflectVec = 0.5f * ssReflectVec + 0.5f;

	// Sampling Position
	vec2 samplingPosition;

	// Depth informations
	float fragmentDepth;
	float rayDepth;
	float sampledDepth;
	

	// Ray tracing while in screen space
	int count = 0;
	while(count < 100)
	{
		// Sampling Position
		samplingPosition = fragment + vsReflectVec.xy;

		// Depth informations
		fragmentDepth = linearizeDepth( texture(deferredDepthTex, fragment) );
		rayDepth = vsReflectVec.z;
		sampledDepth = linearizeDepth( texture(deferredDepthTex, samplingPosition) );
		
		if(rayDepth > sampledDepth)
		{
			fragmentColor = texture(deferredDiffuseTex, samplingPosition);
			break;	
		}
		
		//vsReflectVec += vsReflectVec * stepSize; 
		count++;		
	}
	
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
			// Right side
			else if(fragment.x > (divider + offset))
				fragmentColor = vec4(sampledDepth, sampledDepth, sampledDepth, 1.0f);
			// Left side
			else
				fragmentColor = vec4(rayDepth, rayDepth, rayDepth, 1.0f);	
		}
		// Debugging reflection vectors
		if(showReflVecs)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			// Right side
			else if(fragment.x > (divider + offset))
				fragmentColor = vec4(vsNormal, 1.0f);
			// Left side
			else
				fragmentColor = vec4(ssReflectVec, 1.0f);	
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
	// Depth
	else if(textureID == 6){
		FragColor = vec4( linearizeDepth( float(texture(deferredDepthTex, vert_UV)) ) );
	}

}