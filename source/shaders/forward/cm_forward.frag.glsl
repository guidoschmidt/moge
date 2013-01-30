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

/*** Input ********************************************************************/
in vec4 vert_Position;
in vec4 vert_Normal;
in vec2 vert_UV;
in vec3 ReflectDir;

/*** Output *******************************************************************/
layout( location = 0 ) out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform LightInfo Light;
uniform MouseInfo Mouse;
uniform ScreenInfo Screen;

uniform float Shininess;

uniform bool mouseLight;
uniform bool boundingboxes;

uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

uniform sampler2D colorTex;

uniform samplerCube cubeMapTex;
uniform bool drawSkyBox;

/*** Functions ****************************************************************/
vec3 diffuseShading(vec4 position, vec4 normal, vec4 lightPosition, vec3 diffuseColor)
{
	vec4 lightVector = normalize(lightPosition - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	vec3 shaded = Light.Diffuse * diffuseColor * cosinus + Light.Specular * diffuseColor * pow(cosinus, Shininess);
	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{
	vec4 lightPosition = ProjectionMatrix * ViewMatrix * Light.Position;
	vec3 diffuseColor = texture(colorTex, vert_UV).rgb; 	
	
	if(mouseLight)
	{
		float mouseX = Mouse.X/Screen.Width;
		float mouseY = Mouse.Y/Screen.Height;
		mouseX = ((mouseX - 0.5f) * 2.0f);
		mouseY = ((mouseY - 0.5f) * 2.0f);
		
		lightPosition.x += 20.0f * mouseX;
		lightPosition.z += 20.0f * mouseY;
	}
	
	vec4 diffuseShaded = vec4(diffuseShading(vert_Position, vert_Normal, lightPosition, diffuseColor).rgb, 1.0f);
	vec4 cubeMapColor = texture(cubeMapTex, ReflectDir);
	
	FragColor = cubeMapColor;
}