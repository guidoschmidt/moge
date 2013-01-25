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
uniform bool blur;

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
	// Linerization
	return (2.0 * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

/******************************************************************************/
/* SSR (screen space reflections)
 * @date 	22.01.13
 * @author	Guido Schmidt
 */
vec4 reflectShading()
{
	// Variables
	vec4 reflectionColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate view vector
	vec3 cameraPosition = Camera.Position;
	vec3 shadedPosition = vec3(texture(deferredPositionTex, vert_UV));
	vec3 shadedNormal = texture(deferredNormalTex, vert_UV).xyz;
	
	// Reflect view vector at the normal of current position to get reflection vector
	vec3 viewVector = normalize(shadedPosition - cameraPosition);
	vec4 reflectionVector = reflect(vec4(viewVector, 0.0f), vec4(shadedNormal, 0.0f));
	// Convert reflection vector into screen space
	vec3 ssReflectionVector = normalize((ProjectionMatrix * reflectionVector)).xyz;

	// Get pixel size
	vec2 fragSize = vec2(1.0f/Screen.Width, 1.0f/Screen.Height);
	fragSize += fragSize/2;
	
	// Initialze traced ray
	float stepSize = 0.015f;//min(fragSize.x, fragSize.y);
	vec3 rayStep = ssReflectionVector * stepSize;
	vec3 tracedRay = ssReflectionVector * stepSize;
	
	// Find sampling position
	vec2 sampledPosition = vert_UV + tracedRay.xy;
	
	// Get depths of ray and sampled position
	//***************************************
	// Depth at current fragment
	float fragmentDepth = linearizeDepth(float(texture(deferredDepthTex, vert_UV)));
	// Depth at sampled position
	float sampledDepth = linearizeDepth(float(texture(deferredDepthTex, sampledPosition)));
	// Depth at ray position
	float rayDepth = fragmentDepth + tracedRay.z;
	
	// Raytrace while ray is inside of screen space
	while(	sampledPosition.x <= 1.0f && sampledPosition.x >= 0.0f &&
			sampledPosition.y <= 1.0f && sampledPosition.y >= 0.0f)
	{
		// Update values
		//***************************************
		// Sampling position
		sampledPosition = vert_UV + tracedRay.xy;
		// Sampled depth
		sampledDepth = linearizeDepth(float(texture(deferredDepthTex, sampledPosition)));
		// Ray depth
		rayDepth = fragmentDepth + tracedRay.z * fragmentDepth;
		
		//! If ray depth gets bigger than sampled depth at the current
		//! intersection fragment of screen space
		if(rayDepth > sampledDepth)
		{
			if((rayDepth - sampledDepth) < deltaDepth)
			{
				// Blur implemented as simple averaging along x and y axis
				if(blur)
				{
					// @TODO Jittering, blur in negative & positive x,y-axis
					vec3 sum = vec3(0.0f);
					float size = 0.01f;
					for(float i = -0.01f; i < size; i+=0.001)
					{
						vec2 pos = sampledPosition;
						pos += i;
						sum += vec3(texture(deferredDiffuseTex, pos));
					}
					sum /= (2 * size) * 1000;
					
					reflectionColor = vec4(sum, 1.0f);
				}
				// If not blurred get diffuse lighted color from texture of deferred step
				else
				{
					reflectionColor = texture(deferredDiffuseTex, sampledPosition);
				}
				
			}
			// Break when depth inztersection is found
			break;		
		}
		
		// Extend traced ray by one 'ray step'
		tracedRay += tracedRay * (1.0f + rayStep);
	}
	
	return reflectionColor;
}


/******************************************************************************/
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
				FragColor = reflectance * reflectShading() + (1.0f - reflectance) * texture(deferredDiffuseTex, vert_UV);
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