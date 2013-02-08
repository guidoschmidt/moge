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

//Convert something in camera space to screen space
vec3 convertCameraSpaceToScreenSpace(in vec3 cameraSpace)
{
	vec4 clipSpace = ProjectionMatrix * vec4(cameraSpace, 1);
	vec3 NDCSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5 * NDCSpace + 0.5;
	return screenSpace;
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
	float initalStep = 0.01f;
	float refineStep = 0.7f;
	int maxRefineSteps = 3;
	float stepSize = 0.005f; //1.0f/Screen.Width;
	float blurSize = 1.0f;

	// Current fragment
	vec2 fragment = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
	vec2 ssfragment = 2.0f * fragment - 1.0f;
	vec3 ssPosition = vec3(ssfragment, 0.0f);
	ssPosition.z = linearizeDepth( texture(deferredDepthTex, vert_UV) ); 

	// Normal & position
	vec3 vsPosition = texture(deferredPositionTex, fragment).xyz;
	vec3 vsNormal = normalize(texture(deferredNormalTex, fragment).xyz);
	
	// View vector
	vec3 vsViewVec = normalize(-ssPosition);
	// Reflection vector
	vec3 vsReflectVec =  normalize( reflect(vsViewVec, vsNormal) );

	vec3 vsReflectVecPos = ssPosition + vsReflectVec;
	//vsReflectVec = 2.0f * vsReflectVec - 1.0f;
	vsReflectVec = initalStep * normalize(vsReflectVec);
	vec3 ssReflectVecPos = convertCameraSpaceToScreenSpace(vsReflectVecPos);
	vec3 ssReflectVec = initalStep * normalize(ssReflectVecPos);

	vec3 lastPosition = ssPosition + vsReflectVec;
	vec3 currPosition = lastPosition + vsReflectVec; 
	
	int count = 0;
	int refineSteps = 0;

	vec2 samplePos = currPosition.xy;
	float currDepth = currPosition.z;
	float sampledDepth = linearizeDepth( texture(deferredDepthTex, samplePos).z );
	
	
	while(count < 100)
	{
		if(currPosition.x < 0 || currPosition.x > 1 ||
		   currPosition.y < 0 || currPosition.y > 1)
			break;

		samplePos = 2.0f * (0.5f * currPosition.xy + 0.5f) - 1;
		currDepth = 1 - linearizeDepth(currPosition.z);
		sampledDepth = linearizeDepth( texture(deferredDepthTex, samplePos).z );

		float delta = currDepth - sampledDepth;
		float error = length(vsReflectVec);
		if(currDepth > sampledDepth)
		{
			fragmentColor = texture(deferredDiffuseTex, samplePos);
			break;
			/*
			ssReflectVec *= refineStep;
			currPosition = lastPosition;
			refineSteps++;


			if(refineSteps >= maxRefineSteps)
			{
				vec3 normalAtPos = texture(deferredNormalTex, samplePos).xyz;
				float orientation = dot(vsReflectVec, normalAtPos);
					float cosAngIncidence = -dot(vsViewVec, vsNormal);
					cosAngIncidence = clamp(1 - cosAngIncidence, 0.0, 1.0);
					fragmentColor = texture(deferredDiffuseTex, samplePos) * cosAngIncidence;
				break;
			}
			*/
		}

		lastPosition = currPosition;
		currPosition = lastPosition + vsReflectVec;
		count++;
	}

	//fragmentColor = vec4(vsReflectVecPos, 1.0f);

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
				fragmentColor = vec4(currDepth, currDepth, currDepth, 1.0f);	
		}
		// Debugging reflection vectors
		if(showReflVecs)
		{
			if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
				fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			// Right
			else if(fragment.x > (divider + offset))
				fragmentColor = texture(deferredReflectanceTex, samplePos);
			// Left
			else
				fragmentColor = vec4(currPosition, 1.0f);	
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