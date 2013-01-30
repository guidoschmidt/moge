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
out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform LightInfo Light;
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform MouseInfo Mouse;

uniform int textureID;

uniform bool mouseLight;

uniform float Shininess;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredDepthTex;

/*** Functions ****************************************************************/
vec3 diffuseShading(vec3 position, vec3 normal, vec3 diffuseColor, vec4 lightPos)
{
	vec3 lightVector = normalize(vec3(lightPos) - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	vec3 shaded = Light.Diffuse * diffuseColor * cosinus + Light.Specular * diffuseColor * pow(cosinus, Shininess);
	
	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{	// Compositing
	// Lightposition to view space, and control it with mouse
	vec4 lightPosition = ProjectionMatrix * ViewMatrix * Light.Position;
	
	if(mouseLight)
	{
		float mouseX = Mouse.X/Screen.Width;
		float mouseY = Mouse.Y/Screen.Height;
		mouseX = ((mouseX - 0.5f) * 2.0f);
		mouseY = ((mouseY - 0.5f) * 2.0f);
	
		lightPosition.x += 20.0f * mouseX;
		lightPosition.z += 20.0f * mouseY;
	}
	
	// Gather G-Buffer information from textures
	vec3 position = vec3(texture(deferredPositionTex, vert_UV));
	vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
	vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
	float depth = float(texture(deferredDepthTex,vert_UV));

	// Shading
	// Diffuse
	FragColor = vec4(diffuseShading(position, normal, diffuseColor, lightPosition), 1.0f);
}