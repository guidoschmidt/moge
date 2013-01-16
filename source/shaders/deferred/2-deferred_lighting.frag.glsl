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
struct MouseInfo
{
	float X;
	float Y;
};
struct CameraInfo
{
	mat4 View;
	mat4 Projection;
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
uniform MouseInfo Mouse;
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
	vec4 clipSpace = Camera.Projection * Camera.View * vec4(vector, 1.0f);
	vec3 canonicVolumeSpace = clipSpace.xyz / clipSpace.w;
	vec3 screenSpace = 0.5f * canonicVolumeSpace + 0.5f;
	
	return screenSpace;
}

vec3 reflectionShading()
{
	return vec3(texture(deferredColorTex, 1.0f - vert_UV));
}

vec3 diffuseShading(vec3 position, vec3 normal, vec3 diffuseColor, vec4 lightPos)
{
	vec3 lightVector = normalize(vec3(lightPos) - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	vec3 shaded = Light.Diffuse * diffuseColor * cosinus + Light.Specular * diffuseColor * pow(cosinus, Shininess);
	
	return shaded;
}

// MAIN
void main(void)
{	// Compositing
	if(textureID == -1)
	{
		// Lightposition to view space, and control it with mouse
		vec4 lightpos = Camera.Projection * Camera.View * Light.Position;
		lightpos.x = 5.0f * Mouse.X;
		lightpos.y = 3.0f;
		lightpos.z = 5.0f * Mouse.Y;
		
		// Gather G-Buffer information from textures
		vec3 position = vec3(texture(deferredPositionTex, vert_UV));
		vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
		vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
		float reflectance = float(texture(deferredReflectanceTex,vert_UV));
		float depth = float(texture(deferredDepthTex,vert_UV));
		
		// Shading
		// Diffuse
		FragColor = vec4(diffuseShading(position, normal, diffuseColor, lightpos), 1.0f);
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