#version 400

/*** Uniform block definitions ************************************************/
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
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

/*** Input ********************************************************************/
in vec2 vert_UV;

/*** Output *******************************************************************/
layout (location  = 0) out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform LightInfo Light;
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform MouseInfo Mouse;

uniform int textureID;

uniform float Shininess;

uniform vec3 lightPositions[2];
uniform vec3 lightDifusse[2];

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;

/*** Functions ****************************************************************/
vec3 diffuseShading(vec3 position, vec3 normal, vec3 diffuseColor, vec3 lightPos, vec3 lightDiffuse)
{
	vec3 shaded = vec3(0.0f);

	vec3 lightVector = normalize(lightPos - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	shaded = lightDiffuse * diffuseColor * cosinus + Light.Specular * diffuseColor * pow(cosinus, Shininess);

	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{	// Compositing
	// Lightposition to view space, and control it with mouse
	vec3 lightPosition = ( ProjectionMatrix * ViewMatrix * Light.Position ).xyz;
	
	// Gather G-Buffer information from textures
	vec3 position = vec3(texture(deferredPositionTex, vert_UV));
	vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
	vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
	vec3 materialID = texture(deferredMaterialIDTex, vert_UV).rgb;

	// Shading
	// Diffuse
	vec4 shaded = vec4(0.0f);
	
	// Lights are shaded only with light color
	if(materialID == 0.0f)
		shaded = vec4(Light.Diffuse, 1.0f);
	// Everything else is shaded with diffuse shading	
	else
	{
		for(int i=0; i < lightPositions.length(); i++)
		{
			lightPosition = vec3( ProjectionMatrix * ViewMatrix * vec4(lightPositions[i], 1.0f) );
			shaded += vec4(diffuseShading(position, normal, diffuseColor, lightPosition, lightDifusse[i]), 1.0f);
		}
	}

	FragColor = shaded;
}