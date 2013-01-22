#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
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

uniform bool SSR;
uniform bool switcher;

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

vec4 bilateralFilter() {
	int kernelSize = 10;
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec2 center = gl_FragCoord.xy;
	vec2 sampledFrag;
	float sum = 0.0;
	float coefG, coefZ, finalCoef;
	float Zp = texture(deferredDepthTex, center).z;

	const float epsilon = 0.01;

	for(int i = -(kernelSize - 1)/2; i <= (kernelSize - 1)/2; i++) {
		for(int j = -(kernelSize - 1)/2; j <= (kernelSize - 1)/2; j++) {
		sampledFrag = center + vec2(i, j) / Screen.Width;
		coefG = 1.2f;
		float zTmp = texture(deferredDepthTex, sampledFrag).z;
		coefZ = 1.0 / (epsilon + abs(Zp - zTmp));
		finalCoef = coefG * coefZ;
		sum += finalCoef;
		color += finalCoef * texture(deferredDiffuseTex, sampledFrag);
		}
	}
	
	return color;
} 

vec2 getScreenSpacePosition()
{
	return gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
}

vec3 viewSpaceToScreenSpace(vec3 vector)
{
	vec4 clipSpace = ProjectionMatrix * ViewMatrix * vec4(vector, 1.0f);
	vec3 NDCSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5f * NDCSpace + 0.5f;
	return screenSpace;
}

float linearizeDepth2(float depth)
{
	// Linerization
	float linearDepth = 1.0f/Camera.FarPlane / ((Camera.FarPlane/(Camera.FarPlane - Camera.NearPlane)) - depth);
	return linearDepth;
}

float linearizeDepth(float depth)
{
	// Linerization
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
	while(count < 200)
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

// New SSR
vec4 reflectShading()
{
	// Variables
	vec4 reflectionColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate view vector
	vec3 cameraPosition = Camera.Position;
	vec3 shadedPosition = texture(deferredPositionTex, vert_UV).xyz;
	vec3 shadedNormal = texture(deferredNormalTex, vert_UV).xyz;
	
	// Reflect view vector at the normal of current position to get reflection vector
	vec3 viewVector = normalize(shadedPosition - cameraPosition);
	vec4 reflectionVector = reflect(vec4(viewVector, 0.0f), vec4(shadedNormal, 0.0f));
	// Convert reflection vector into screen space
	vec3 ssReflectionVector = normalize(vec3(ProjectionMatrix * ViewMatrix * reflectionVector));
	
	// Get pixel size
	vec2 fragSize = vec2(1.0f/Screen.Width, 1.0f/Screen.Height);
	//fragSize += fragSize/2;
	// Initialze traced ray
	float stepSize = min(fragSize.x, fragSize.y);
	vec3 rayStep = ssReflectionVector * stepSize;
	vec3 tracedRay = ssReflectionVector * stepSize;
	
	// Find sampling position
	vec2 sampledPosition = vert_UV - tracedRay.xy;
	
	// Get depths of ray and sampled position
	//***************************************
	// Depth at current fragment
	float fragmentDepth = linearizeDepth(float(texture(deferredDepthTex, vert_UV)));
	// Depth at sampled position
	float sampledDepth = linearizeDepth(float(texture(deferredDepthTex, sampledPosition)));
	// Depth at ray position
	float rayDepth = fragmentDepth + tracedRay.z;
	
	// Raytrace while ray is inside of screen space
	while(	sampledPosition.x >= -1.0f && sampledPosition.x <= 1.0f &&
			sampledPosition.y >= -1.0f && sampledPosition.y <= 1.0f)
	{
		// Update values
		//***************************************
		// Sampling position
		sampledPosition = vert_UV + tracedRay.xy;
		// Sampled depth
		sampledDepth = linearizeDepth(float(texture(deferredDepthTex, sampledPosition)));
		// Ray depth
		rayDepth = fragmentDepth + tracedRay.z;
			
		if(rayDepth >= sampledDepth)
		{
			if((rayDepth - fragmentDepth) < deltaDepth)
			{
				reflectionColor = texture(deferredDiffuseTex, sampledPosition);
				break;		
			}
		}
		
		tracedRay += rayStep;
	}
	if(switcher)
		reflectionColor = vec4(ssReflectionVector.z, ssReflectionVector.z, ssReflectionVector.z, 1.0f);
	
	return reflectionColor;
}

// Working SSR
vec4 traceReflection(vec4 reflectionVector, vec2 normalisedResolution, float startDepth)
{

	reflectionVector.z = linearizeDepth(reflectionVector.z);
	float size = length(reflectionVector.xy);
	//reflectionVector = (reflectionVector / size);
	reflectionVector = reflectionVector * max(normalisedResolution.x, normalisedResolution.y);
	
	vec2 currentTexcord = vert_UV;
	float currentDepth = startDepth;
	
	while(currentTexcord.x <= 1.0 && currentTexcord.x >= 0.0 && currentTexcord.y <= 1.0 && currentTexcord.y >= 0.0){
		currentTexcord = currentTexcord + reflectionVector.xy;
		currentDepth = currentDepth + reflectionVector.z;
		float tmpDepth = linearizeDepth( texture(deferredDepthTex, currentTexcord).x );
		
		if(currentDepth >= tmpDepth ){
			if(currentDepth-tmpDepth < deltaDepth){
				vec2 uv = currentTexcord;
				vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
				color += texture(deferredDiffuseTex, uv);
		
				return color;
				//return vec4(currentTexcord - textureCordinate,currentDepth - startDepth,1);
			}else{
				return texture(deferredDiffuseTex, vert_UV);
			}
		}
	}
	return texture(deferredDiffuseTex, vert_UV);
}

// Working SSR
vec4 reflectionPass()
{
	vec3 realCameraPosition = Camera.Position; // korrektur von falschen kammera daten XD
	
	vec2 normalisedResolution = vec2(1.0f/Screen.Width, 1.0f/Screen.Height);
	normalisedResolution.x += normalisedResolution/2;
	normalisedResolution.y += normalisedResolution/2;
	
	vec3 localNormal = normalize(texture(deferredNormalTex, vert_UV).xyz);
	vec4 localColor = texture(deferredDiffuseTex, vert_UV);
	localColor.a = 0.0f;
	
	vec4 reflectedColor = vec4(0, 0, 0, 1);
	
	//if( localNormal.x != 0.0 || localNormal.y != 0.0 || localNormal.z != 0.0){
	if(localNormal.y != 0.0 || localNormal.z != 0.0){
		vec3 localPosition = texture(deferredPositionTex, vert_UV).xyz;
		float localDepth = linearizeDepth(texture(deferredDepthTex, vert_UV).z);
	
		vec3 lookVector = normalize(localPosition - realCameraPosition);
		vec4 reflectionVector = ProjectionMatrix * ViewMatrix * vec4(reflect(lookVector, localNormal), 0.0f);
		
		reflectedColor = traceReflection( reflectionVector, normalisedResolution, localDepth );
		//reflectedColor = reflectionVector;
	}else{
		//vec3 lightVector = normalize( vec3(1, -1, -0.5) )*-1;
	    //reflectedColor = clamp( dot( lightVector, localNormal ) * localColor, 0.0, 1.0 ) * 0.8 + 0.3 * localColor;
	    //reflectedColor.a = 0.0;
	}
	// performing some kind of lighting!
	//vec3 lightVector = normalize( vec3(1, -1, -0.5) )*-1;
	//localColor = clamp( dot( lightVector, localNormal ) * localColor, 0.0, 1.0 ) * 0.8 + 0.3 * localColor;
	//localColor.a = 0.0;
	
	localColor = reflectedColor;
	
	return reflectedColor;
}

/*** Main *********************************************************************/
void main(void)
{	
	// Compositing
	if(textureID == -1)
	{
		float reflectance = float(texture(deferredReflectanceTex, vert_UV));
		// Diffuse shading
		FragColor = texture(deferredDiffuseTex, vert_UV);
		// Reflections
		if(SSR)
		{
			if(reflectance > 0.0f)
			{
				//vec4 reflShaded = reflectionPass(); // + (1 - reflectance) * texture(deferredDiffuseTex, vert_UV);
				// Working SSR
				//FragColor = reflectionPass();
				// 2nd (new) Attempt SSR
				FragColor = reflectShading();
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
		FragColor = texture(deferredReflectanceTex, vert_UV);
	// Depth
	else if(textureID == 5)
		FragColor = texture(deferredDepthTex, vert_UV);
}