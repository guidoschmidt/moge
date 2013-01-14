#version 400

// TYPEDEFS
struct LightInfo
{
	vec4 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
struct ScreenInfo
{
	float Width;
	float Height;
};
struct CameraInfo
{
	mat4 CameraToClipMatrix;
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

// INS
in vec2 vert_UV;

// OUTS
out vec4 FragColor;

// UNIFORMS
uniform LightInfo Light;
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform float Shininess;
uniform int textureID;
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredDepthTex;

// FUNCTIONS
vec3 viewSpaceToScreenSpace(vec3 vector)
{
	vec4 clipSpace = Camera.CameraToClipMatrix * vec4(vector, 1.0f);
	vec3 canonicVolumeSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5f * canonicVolumeSpace + 0.5f;
	
	return screenSpace;
}

vec3 reflectionShadingOld(vec3 normal)
{
	// Parameters
	int maxSamples = 200;
	float stepSize = 0.1f;
	int maxRefinements = 3;
	
	// Variable initialization
	vec3 shaded = vec3(0.0f, 0.0f, 0.0f);
	int sampleCount = 0;
	int numRefinements = 0;

	vec2 ssPosition2D = gl_FragCoord.xy/vec2(Screen.Width, Screen.Height);
	vec3 vsPosition = vec3(texture(deferredPositionTex, ssPosition2D));
	vec3 vsNormal = vec3(texture(deferredNormalTex, ssPosition2D));


	// vs = view space
	vec3 vsViewDir = normalize(vsPosition);
	vec3 vsReflectionVector = reflect(vsViewDir, vsNormal);
	
	vec3 ssPosition = viewSpaceToScreenSpace(vsPosition);
	
	vec3 vsReflectionVectorPosition = vsPosition + vsReflectionVector;
	vec3 ssReflectionVectorPosition = viewSpaceToScreenSpace(vsReflectionVectorPosition);
	vec3 ssReflectionVector = normalize(ssReflectionVectorPosition - ssPosition);
	ssReflectionVector = stepSize * ssReflectionVector;
	
	vec3 oldPos = ssPosition + ssReflectionVector;
	vec3 curPos = oldPos + ssReflectionVector;
	
	while(sampleCount < maxSamples)
	{
		
		vec2 sampledPos = curPos.xy;	
		float rayDepth = curPos.z;
		
		float sampledDepth = texture(deferredDepthTex, sampledPos).z;
		float delta = rayDepth - sampledDepth;
		shaded = vec3(sampledDepth);

		if(delta >= 2.0f)
		{
			ssReflectionVector *= stepSize;
			curPos = oldPos;
			numRefinements++;
			if(numRefinements >= maxRefinements)
			{
				vec3 normalAtPos = texture(deferredNormalTex, sampledPos).xyz;
				shaded = vec3(texture(deferredColorTex, sampledPos));
				shaded = vec3(0.5f, 1.0f, 0.25f);
				break;
			}
		}
		
		oldPos = curPos;
		curPos = oldPos + ssReflectionVector;
		sampleCount++;
	}

	return shaded;
}


vec3 reflectionShading(vec3 normal, float depth)
{
	vec3 shaded = vec3(0.0f, 0.0f, 0.0f);
	
	vec3 viewVector = normal - Camera.Position;
	vec3 reflectionVector = reflect(normal, viewVector);
	
	reflectionVector = viewSpaceToScreenSpace(reflectionVector);
	

	return vec3(reflectionVector.z); 
}

vec3 diffuseShading(vec3 position, vec3 normal, vec3 diffuseColor)
{
	vec3 lightVector = normalize(vec3(Light.Position) - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	vec3 shaded = Light.Ambient + Light.Diffuse * diffuseColor * cosinus + Light.Specular * pow(cosinus, Shininess);
	
	return shaded;
}

// MAIN
void main(void)
{	// Composited
	if(textureID == -1)
	{
		vec3 position = vec3(texture(deferredPositionTex, vert_UV));
		vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
		vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
		float reflectance = float(texture(deferredReflectanceTex,vert_UV));
		float depth = float(texture(deferredDepthTex,vert_UV));
		// Shading	 
		FragColor = vec4(diffuseShading(position, normal, diffuseColor), 1.0f);
		if(reflectance > 0.0f)
			FragColor = vec4(reflectionShading(normal, depth), 1.0f);
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