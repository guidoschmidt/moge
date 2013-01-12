#version 400

// TYPEDEFS
struct LightInfo
{
	vec4 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
struct WindowInfo
{
	float Width;
	float Height;
};
struct CameraInfo
{
	vec4 Position;
	vec3 LookAt;
};

// INS
in vec2 vert_UV;

// OUTS
out vec4 FragColor;

// UNIFORMS
uniform LightInfo Light;
uniform CameraInfo Camera;
uniform WindowInfo Window;
uniform float Shininess;
uniform int textureID;
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredDepthTex;

// FUNCTIONS
vec3 reflectionShading(vec3 position, vec3 normal, vec3 diffuseColor, float depth)
{
	vec3 viewVector = normalize(vec3(Camera.Position) - position);
	vec3 reflectVector = reflect(viewVector, normal);
	vec3 shaded = reflectVector;
	if(reflectVector.z == depth){
		vec3 shaded = diffuseColor;
	}
	return shaded;
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
{
	if(textureID == -1)
	{
		vec3 pos = vec3(texture(deferredPositionTex, vert_UV));
		vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
		vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
		float reflectance = float(texture(deferredReflectanceTex,vert_UV));
		float depth = float(texture(deferredDepthTex,vert_UV));
		if(reflectance > 0.0f)
			FragColor = vec4(reflectionShading(pos, normal, diffuseColor, depth), 1.0f);
		else
			FragColor = vec4(diffuseShading(pos, normal, diffuseColor), 1.0f);
	}
	else if(textureID == 0)
		FragColor = texture(deferredPositionTex, vert_UV);
	else if(textureID == 1)
		FragColor = texture(deferredColorTex, vert_UV);
	else if(textureID == 2)
		FragColor = texture(deferredNormalTex, vert_UV);
	else if(textureID == 3)
		FragColor = texture(deferredMaterialIDTex, vert_UV);
	else if(textureID == 4)
		FragColor = texture(deferredReflectanceTex, vert_UV);
	else if(textureID == 5)
		FragColor = texture(deferredDepthTex, vert_UV);
}