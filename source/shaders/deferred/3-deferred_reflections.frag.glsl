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

uniform float rayStepSize;

uniform bool toggleSSR;
uniform bool blur;
uniform bool compareDepth;
uniform bool showReflVecs;
uniform bool jittering;
uniform bool mouseSlider;
	
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

// Variables for displaying 2 textures with divider on screen 
float divider = Mouse.X/Screen.Width;
float offset = 0.0009f;

/*** Functions ****************************************************************/ 
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	return (2.0 * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
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
vec3 raytrace(in vec3 reflectionVector, in vec2 normalizedResolution, in float startDepth)
{
	vec3 color = vec3(0.0f);
	float stepSize = rayStepSize;

	float size = length(reflectionVector.xy);
	reflectionVector = reflectionVector/size;
	reflectionVector = reflectionVector * stepSize;

	vec2 uv = vert_UV;
	float currentDepth = startDepth;
	float sampledDepth = linearizeDepth( texture(deferredDepthTex, uv).z );

	while(uv.x <= 1.0 && uv.x >= 0.0 &&
	      uv.y <= 1.0 && uv.y >= 0.0)
	{
		uv = uv + reflectionVector.xy;
		currentDepth = currentDepth + reflectionVector.z * startDepth;
		float sampledDepth = linearizeDepth( texture(deferredDepthTex, uv).z );

		if(currentDepth > sampledDepth)
		{
			float delta = (currentDepth - sampledDepth);
			if(delta < min(rayStepSize, length(reflectionVector) ) )
			{
				color = texture(deferredDiffuseTex, uv).rgb;
				break;
			}
		}
		else
		{
		 	color = texture(deferredDiffuseTex, vert_UV).rgb;
		}
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

	vec2 normalizedResolution = vec2(1/Screen.Width, 1/Screen.Height);
	vec3 normal = normalize( texture(deferredNormalTex, vert_UV) ).xyz;

	float localDepth = linearizeDepth( texture(deferredDepthTex, vert_UV).z );

	vec3 lookVector = normalize( vec3(0.0f, 0.0f, Camera.NearPlane) );
	vec3 reflectionVector = ( ProjectionMatrix * reflect( vec4(-lookVector, 0), vec4(normal, 0) ) ).xyz;


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
			reflectedColor = reflectionVector;	
	}
	// No debugging
	else
	{
		reflectedColor = raytrace(reflectionVector, normalizedResolution, localDepth);	
	}
	

	return vec4(reflectedColor, 1.0f);
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
		if( (mouseSlider && showReflVecs) || (mouseSlider && compareDepth) )
		{
			FragColor = SSR();
		}

		//FragColor = vec4(rayStepSize);

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