#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
	mat4 View;
	mat4 Projection;
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
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

uniform int textureID;

uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredDepthTex;
uniform sampler2D deferredDiffuseTex;


/*** Functions ****************************************************************/
vec2 getScreenSpacePosition()
{
	return gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
}

vec3 viewSpaceToScreenSpace(vec3 vector)
{
	vec4 clipSpace = Camera.Projection * Camera.View * vec4(vector, 1.0f);
	vec3 NDCSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5f * NDCSpace + 0.5f;
	return screenSpace;
}

float linearizeDepth(float depth)
{
	return (2.0 * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

vec4 ComputeReflection()
{
	// Variables
	float initialStepAmount = .01;
	float stepRefinementAmount = .7;
	int maxRefinements = 3;
	int maxDepth = 1;
	
	// Values from textures
	vec2 screenSpacePosition2D = vert_UV;
	vec3 cameraSpacePosition = texture(deferredPositionTex, screenSpacePosition2D).xyz;
	vec3 cameraSpaceNormal = texture(deferredNormalTex, screenSpacePosition2D).xyz;

	// Screen space vector
	vec3 cameraSpaceViewDir = normalize(cameraSpaceNormal - Camera.Position);
	vec3 cameraSpaceVector = normalize(reflect(cameraSpaceViewDir, cameraSpaceNormal));
	vec3 screenSpacePosition = viewSpaceToScreenSpace(cameraSpacePosition);
	vec3 cameraSpaceVectorPosition = cameraSpacePosition + cameraSpaceVector;
	vec3 screenSpaceVectorPosition = viewSpaceToScreenSpace(cameraSpaceVectorPosition);
	vec3 screenSpaceVector = initialStepAmount * normalize(screenSpaceVectorPosition - screenSpacePosition);
	
	vec3 oldPosition = screenSpacePosition + screenSpaceVector;
	vec3 currentPosition = oldPosition + screenSpaceVector;
	
	vec4 color = vec4(0,0,0,1);
	int count = 0;
	int numRefinements = 0;
	int depth = 0;

	// Raytracing
	while(count < 500)
	{
		//Stop ray trace when it goes outside screen space
		if(currentPosition.x < 0 || currentPosition.x > 1 ||
		   currentPosition.y < 0 || currentPosition.y > 1 ||
		   currentPosition.z < 0 || currentPosition.z > 1)
			break;

		//intersections
		vec2 samplePos = currentPosition.xy;
		float currentDepth = linearizeDepth(currentPosition.z);
		float sampleDepth = linearizeDepth(texture(deferredDepthTex, samplePos).x);
		float diff = currentDepth - sampleDepth;
		float error = 0.25f;
		if(diff >= 0)
		{
			screenSpaceVector *= stepRefinementAmount;
			currentPosition = oldPosition;
			numRefinements++;
			if(numRefinements >= maxRefinements)
			{
				color = texture(deferredDiffuseTex, samplePos);
				break;
			}
		}

		//Step ray
		oldPosition = currentPosition;
		currentPosition = oldPosition + screenSpaceVector;
		count++;
	}
	
	return color;
}

vec3 reflectionPass()
{
	// Variables
	vec3 shaded = vec3(0.0f, 0.0f, 0.0f);
	float stepSize = 0.01f;
	int numSteps = 0;
	
	vec2 samplePosition = getScreenSpacePosition();
	
	vec3 vsViewPosition = vec3(Camera.Projection * Camera.View * vec4(Camera.Position, 1.0f));
	vec3 vsNormal = vec3(texture(deferredNormalTex, samplePosition));
	vec3 vsPosition = vec3(texture(deferredPositionTex, samplePosition));
	vec3 vsViewVec = vsPosition - vsViewPosition;
	
	vec3 vsReflectionVec = reflect(normalize(vsViewVec), normalize(vsNormal));
	
	vec3 ssReflectionVec = viewSpaceToScreenSpace(vsReflectionVec);

	float rayDepth = ssReflectionVec.z;
	float sampledDepth = float(texture(deferredDepthTex, ssReflectionVec.xy));
	float delta = sampledDepth - rayDepth;
	
	if(delta < 0.01f)
	{
		shaded = vec3(texture(deferredDiffuseTex, ssReflectionVec.xy));
	}
		
	//shaded = vec3(sampledDepth);
	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{	
	// Compositing
	if(textureID == -1)
	{
		float reflectance = float(texture(deferredReflectanceTex, vert_UV));
		// Diffuse
		FragColor = texture(deferredDiffuseTex, vert_UV);
		// Reflections
		if(reflectance > 0.0f)
		{
			vec3 reflShaded = vec3(reflectance * ComputeReflection() + (1 - reflectance) * texture(deferredDiffuseTex, vert_UV));
			FragColor = vec4(reflShaded, reflectance);
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
		FragColor = texture(deferredReflectanceTex, vert_UV);
	// Depth
	else if(textureID == 5)
		FragColor = texture(deferredDepthTex, vert_UV);
}