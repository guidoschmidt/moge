#version 400

/*** Uniform block definitions ************************************************/
struct LightInfo
{
	vec3[3] Position;
	vec3[3] Diffuse;
	vec3 Specular;
	int Count;
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

uniform vec3 ambientColor;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D deferredViewPositionTex;
uniform sampler2D deferredWoldPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;

/*** Functions ****************************************************************/
// Diffuse shading function
vec2 diffuseCooef(in float dot )
{
  // Front facing light
  if(dot > 0)
    return vec2(dot, 1.0f);
  else
    return vec2(0.0f, 0.0f); 	// kill the lighting and cascade multiplier = 0
}

// Specular shading function
float specularCooef(in float dot, in float shininess)
{
	// Front facing light
	if(dot > 0)
		return pow(dot, shininess);
	else
		return 0;
}

// Phong shading
vec3 phongShading(in vec3 lightVector, in vec3 normal, in vec3 materialColor, in vec3 lightColorD, in vec3 lightColorS, float shininess)
{	
	vec3 shaded = vec3(0.0f);

	vec2 cD = diffuseCooef(dot(normal, lightVector));
	float cS = specularCooef(dot(normal, lightVector), shininess);

	// Diffuse term
	shaded += lightColorD * materialColor * cD.x; // * Kd;
	// Specular term
	shaded += lightColorS * materialColor * cS; // * Ks; 

	return shaded;
}

/*** Main *********************************************************************/
void main(void)
{	
	// Gather G-Buffer information from textures
	vec4 position = texture(deferredWoldPositionTex, vert_UV);
	vec3 normal = vec3( texture(deferredNormalTex, vert_UV) );
	vec3 materialID = texture(deferredMaterialIDTex, vert_UV).rgb;
	vec3 materialColorD = texture(deferredColorTex, vert_UV).rgb;

	// Shading
	// Ambient term
	vec4 shaded = vec4(ambientColor, 1.0f);
	
	// Lights are shaded only with light color
	// Everything else is shaded with diffuse shading	
	vec3 lightVector;
	for(int i=0; i < Light.Count; i++)
	{
		if(materialID == 0.0f)
			shaded = vec4(materialColorD, 1.0f);
		else
			lightVector = ( normalize( vec4(Light.Position[i], 1.0f) - position) ).xyz;
			shaded += vec4( phongShading(lightVector, normal, materialColorD, Light.Diffuse[i].rgb, Light.Specular.rgb, Shininess), 1.0f );
	}
	

	FragColor = shaded;
}