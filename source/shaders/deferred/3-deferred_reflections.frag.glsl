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

uniform float deltaDepth;
	
uniform	mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
	
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
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
	float initalStep = 0.01f;
	float stepSize = 0.025f;
	float blurSize = 1.0f;

	// Current fragment
	vec2 fragment = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);

	// Normal & position
	vec3 vsNormal = normalize(texture(deferredNormalTex, fragment).xyz);
	vec3 vsPosition = texture(deferredPositionTex, fragment).xyz;
	vec3 ssPosition = (ProjectionMatrix * vec4(vsPosition, 1.0f)).xyz;
	ssPosition = (0.5f * ssPosition) + 0.5f;
	
	// View vector
	vec3 vsViewVec = -ssPosition;

	// Reflection vector
	vec3 vsReflectVec = reflect(normalize(vsViewVec), normalize(vsNormal));
	vsReflectVec = normalize(vsReflectVec);
	vsReflectVec += ssPosition;
	vec3 ssReflectVec = (vec4(vsReflectVec, 0.0f) * ProjectionMatrix).xyz / vsReflectVec.z;
	ssReflectVec = (0.5f * ssReflectVec) + 0.5f;
	ssReflectVec -= ssPosition;

	// Flipping z axis of screen space reflection vector for debugging (e.g. rendering in rgb)
	//ssReflectVec.z *= -1;

	// Initialze traced ray
	vec3 initialRay = ssReflectVec * (initalStep); 

	vec3 tracedRay = initialRay;
	// Get depth informations
	float fragmentDepth = linearizeDepth(texture(deferredDepthTex, fragment)); 
	vec2 samplingPosition = fragment + tracedRay.xy;
	float sampledDepth = linearizeDepth(texture(deferredDepthTex, samplingPosition));
	float rayDepth = fragmentDepth + linearizeDepth(tracedRay.z) / fragmentDepth;

	// Ray tracing while in screen space
	int count = 0;
	while(	samplingPosition.x > 0.0f && samplingPosition.x < 1.0f &&
			samplingPosition.y > 0.0f && samplingPosition.y < 1.0f)
	{
		// Update sampling position and depth values
		samplingPosition = fragment + tracedRay.xy;
		sampledDepth = linearizeDepth(texture(deferredDepthTex, samplingPosition));
		rayDepth = fragmentDepth + linearizeDepth(tracedRay.z) * fragmentDepth;
		
		// intersection found
		if(rayDepth > sampledDepth)
		{
			if(abs(rayDepth - sampledDepth) < 0.002f)
			{
				// Blur implemented as simple averaging along x and y axis
				if(blur)
				{
					// @TODO Jittering, blur in negative & positive x,y-axis
					vec3 sum = vec3(0.0f);
					float size = blurSize/100.0f;
					for(float i = -0.01f; i < size; i+=0.001)
					{
						vec2 pos = samplingPosition;
						pos += i;

						if(jittering)
						{
							float randomX = clamp(rand(gl_FragCoord.xy), 0, 1)/(Screen.Width * 500f);
							float randomY = clamp(rand(gl_FragCoord.yx), 0, 1)/(Screen.Height * 500f);
						 	pos += vec2(randomX, randomY);
						}

						sum += vec3(texture(deferredDiffuseTex, pos));
					}
					sum /= (2 * size) * 1000;
					
					fragmentColor = vec4(sum, 1.0f);
				}
				// No blur
				else{
					fragmentColor = vec4(texture(deferredDiffuseTex, samplingPosition).rgb, 1.0f);	
				}
			}
			// Ray tracing termination
			break;
		}
		else
			fragmentColor = vec4(texture(deferredDiffuseTex, fragment).rgb, 1.0f);


		//Jitter the initial ray
		if(jittering)
		{
			float randomOffset1 = clamp(rand(gl_FragCoord.yx), 0, 1)/(Screen.Width  * 15000f);
			float randomOffset2 = clamp(rand(gl_FragCoord.xy), 0, 1)/(Screen.Height * 15000f);
			float randomOffset3 = clamp(rand(gl_FragCoord.zz), 0, 1)/(fragmentDepth * 15000f);
			tracedRay += vec3(randomOffset1, randomOffset2, 0);
			stepSize += randomOffset3;
		}

		tracedRay += tracedRay * stepSize;
		count++;
	}

	// Variables for displaying 2 textures with divider on screen 
	float divider = Mouse.X/Screen.Width;
	float offset = 0.0009f;
	
	// Debugging depths
	if(compareDepth)
	{
		if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
			fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
		else if(fragment.x > (divider + offset))
			fragmentColor = vec4(sampledDepth, sampledDepth, sampledDepth, 1.0f);
		else
			fragmentColor = vec4(rayDepth, rayDepth, rayDepth, 1.0f);	
	}
	// Debugging reflection vectors
	if(showReflVecs)
	{
		if((fragment.x <= (divider + offset)) && (fragment.x >= (divider - offset)))
			fragmentColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
		else if(fragment.x > (divider + offset))
			fragmentColor = vec4(vsNormal, 1.0f);
		else
			fragmentColor = vec4(tracedRay, 1.0f);	
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
	// Depth
	else if(textureID == 5)
		FragColor = vec4( linearizeDepth( float(texture(deferredDepthTex, vert_UV)) ) );
}