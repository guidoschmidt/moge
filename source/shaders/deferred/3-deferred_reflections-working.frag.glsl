#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
	float FOV;
	float NearPlane;
	float FarPlane;
	vec3 Position;
	vec3 LookAt;
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
	
uniform sampler2D vsPositionTex;
uniform sampler2D wsPositionTex;
uniform sampler2D ColorTex;
uniform sampler2D vsNormalTex;
uniform sampler2D wsNormalTex;
uniform sampler2D MaterialIDTex;
uniform sampler2D ReflectanceTex;
uniform sampler2D vsReflectionVectorTex;
uniform sampler2D DiffuseTex;
uniform sampler2D DepthTex;
uniform sampler2D LinearDepthTex;

// Variables for displaying 2 textures with divider on screen 
float divider = Mouse.X/Screen.Width;
float offset = 0.0009f;

/*** Functions ****************************************************************/ 
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	float n = Camera.NearPlane; // camera z near
	float f = Camera.FarPlane; // camera z far
	float linearDepth =  (2.0 * n) / (f + n - depth * (f - n));

	return linearDepth;
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
vec3 raytrace(in vec3 ssPosition, in vec3 reflectionVector, in float startDepth)
{
	// Variables
	vec3 color = vec3(0.0f);
	
	// Ray step Variables
	vec2 pixelSize       = 1.0/vec2(Screen.Width, Screen.Height);
	float stepSize       = min(pixelSize.x, pixelSize.y); 
	float initalStepSize = 0.002;
	reflectionVector     = normalize(reflectionVector);

	// Initialize ray
	reflectionVector = initalStepSize * reflectionVector;
	vec3 rayStep     = stepSize * reflectionVector;
	vec3 refineStep  = stepSize * reflectionVector;

	// Sample position and depths
	vec2 sampledPosition = vert_UV + reflectionVector.xy;
	float currentDepth   = startDepth + reflectionVector.z / startDepth;
	float sampledDepth   = texture(DepthTex, sampledPosition).z;

	int count = 0;
	while(count < 500 )
	{
		// Update values
		sampledPosition += reflectionVector.xy;
		currentDepth = startDepth + reflectionVector.z / startDepth;
		float sampledDepth = linearizeDepth( texture(DepthTex, sampledPosition).z);

		if(currentDepth > sampledDepth)
		{
			float delta = (currentDepth - sampledDepth);
			if(delta < 0.003f )
			{
				color = texture(DiffuseTex, sampledPosition).rgb;
				break;
			}

			// Refinement steps, doesn't work yet
			/* 
			int refine = 0;
			while(refine < 3)
			{
				float refineDepth = currentDepth + reflectionVector.z;
				sampledDepth      = linearizeDepth( texture(DepthTex, sampledPosition).z );

				if(refineDepth <= sampledDepth)
				{
					color = texture(DiffuseTex, sampledPosition).rgb;
					//break;
				}

				reflectionVector  -= refineStep;
				sampledPosition   += reflectionVector.xy;
				refine++;
			}
			*/
		}

		// Step ray
		reflectionVector += rayStep;
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
	// Reflected color
	vec3 reflectedColor = vec3(0.0f);

	// Depth at current fragment
	float currDepth = linearizeDepth( texture(DepthTex, vert_UV).z );
	
	// View space normal
	vec3 vsNormal = normalize( texture(vsNormalTex, vert_UV) ).xyz;
	// View space position
	vec3 vsPosition = (texture(vsPositionTex, vert_UV) ).xyz;
	vsPosition.xy   = (2.0 * vert_UV - 1.0) * currDepth;
	vsPosition.z    = currDepth;
	
	// Screen space position
	vec4 csPosition   = ProjectionMatrix * vec4(vsPosition, 1.0);
	vec3 ndcsPosition = csPosition.xyz / csPosition.w;
	vec3 ssPosition   = 0.5 * ndcsPosition - 0.5;
	ssPosition.xy = 2.0 * vert_UV - 1.0;
	ssPosition.z = currDepth;

	// View space reflection vector
	vec3 vsEyeVector	= normalize( vec3(0, 0, Camera.NearPlane) );
	vec3 vsReflect 		= (reflect( vec4(vsEyeVector, 0), vec4(vsNormal, 0) ) ).xyz;	
	
	// View space reflection vector from texture
	vec3 reflectionVector = texture(vsReflectionVectorTex, vert_UV).xyz;
	
	// World space reflection vector
	vec3 wsNormal       = texture(wsNormalTex, vert_UV).xyz;
	vec3 wsPosition     = texture(wsPositionTex, vert_UV).xyz;
	vec3 wsEyeVector    = wsPosition - Camera.Position;
	vec3 wsReflect      = reflect( normalize(wsEyeVector), normalize(wsNormal) );

	// Project world space reflection vector to view space
	vec3 vspReflectionVector = ( ViewMatrix * vec4(wsReflect, 0.0) ).xyz;
	vspReflectionVector = normalize(vspReflectionVector);

	// Project view space reflection vector to screen space
	vec4 ssReflectionVector = ProjectionMatrix * vec4(vspReflectionVector, 0.0);
	ssReflectionVector.xyz = ssReflectionVector.xyz / ssReflectionVector.w * 0.5 + 0.5;
	ssReflectionVector.xyz = normalize(ssReflectionVector.xyz);

	// Debugging reflection vectors
	if(mouseSlider && showReflVecs)
	{
		if((vert_UV.x <= (divider + offset)) && (vert_UV.x >= (divider - offset)))
			reflectedColor = vec3(1.0f, 0.0f, 0.0f);
		// Right side
		else if(vert_UV.x > (divider + offset))
			reflectedColor = vsNormal;
		// Left side
		else
			reflectedColor = reflectionVector;	
	}
	// No debugging
	else
	{
		// Ray trace
		reflectedColor = raytrace(ssPosition, ssReflectionVector.xyz, currDepth );	
	}
	
	return vec4(reflectedColor, 1.0f);
}

/*** Main *********************************************************************/
void main(void)
{	
	// Reflection properties
	float reflectance = texture(ReflectanceTex, vert_UV).a;
	// Diffuse shading
	vec4 shaded = texture(DiffuseTex, vert_UV);
	
	// Reflections
	FragColor = shaded;
	if(toggleSSR) // Screen space reflections
	{
		
		FragColor = reflectance * SSR();
		
	}
	if( (mouseSlider && showReflVecs) || (mouseSlider && compareDepth) )
	{
		FragColor = reflectance * SSR();
	}
}