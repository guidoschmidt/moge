#version 400

/*** Uniform block definitions ************************************************/
struct MaterialInfo
{
	int id;
	float reflectance;
};

/*** Input ********************************************************************/
in vec3 vert_Position;
in vec3 vert_Normal;
in vec2 vert_UV;
in vec3 vert_Camera;
in vec3 vert_ReflectDirection;

/*** Output *******************************************************************/
layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Color;
layout (location = 2) out vec3 Normal;
layout (location = 3) out vec3 MaterialIDs;
layout (location = 4) out vec4 Reflectance;

/*** Uniforms *****************************************************************/
uniform MaterialInfo Material;
uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform samplerCube cubeMapTex;

/*** Main *********************************************************************/
void main(void)
{
	Position = vert_Position;
	Color = vec3(texture(colorTex, vert_UV));
	Normal = vert_Normal;
	//Normal = texture(NormalMapTex, vert_UV);
	vec3 cubeMapColor = texture(cubeMapTex, vert_ReflectDirection).rgb;
	Reflectance.rgb = cubeMapColor;
	Reflectance.a = Material.reflectance;
	
	switch(Material.id)
	{
		case 0:
			MaterialIDs.r = 1.0f;
			break;
		case 1:
			MaterialIDs.g = 1.0f;
			break;
		case 2:
			MaterialIDs.b = 1.0f;
			break;
		case 3:
			MaterialIDs.r = 1.0f;
			MaterialIDs.g = 1.0f;
			break;
		case 4:
			MaterialIDs.g = 1.0f;
			MaterialIDs.b = 1.0f;
			break;
		case 5:
			MaterialIDs.b = 1.0f;
			MaterialIDs.r = 1.0f;
			break;
	}	
}