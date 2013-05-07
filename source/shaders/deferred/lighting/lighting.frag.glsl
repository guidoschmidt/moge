//FRAGMENT SHADER
#version 400 core

//*** Uniform block definitions ************************************************
struct LightInfo
{
	vec3[30] Position;
	vec3[30] Diffuse;
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

//*** Input ********************************************************************
in vec2 vert_UV;

//*** Output *******************************************************************
layout (location = 0) out vec4 FragColor;

//*** Uniforms *****************************************************************
uniform LightInfo Light;
uniform CameraInfo Camera;
uniform ScreenInfo Screen;
uniform MouseInfo Mouse;

uniform int textureID;
uniform float Shininess;
uniform vec3 ambientColor;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

//uniform sampler2D wsPositionTex;
//uniform sampler2D wsNormalTex;
uniform sampler2D vsPositionTex;
uniform sampler2D vsNormalTex;
uniform sampler2D ColorTex;

//*** Functions ****************************************************************
// Diffuse shading function
vec2 diffuseCooef(in float dot )
{
  // Front facing light
  if(dot > 0)
    return vec2(dot, 1.0f);
  else
    return vec2(0.0f, 0.0f);
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

// Jan Sobottas lighting
vec4 getLighted(in vec3 lightVector, vec4 color, vec3 normal)
{
	vec4 shadedColor = clamp( dot( lightVector, normal ) * color, 0.0, 1.0 ) * 0.8 + 0.3 * color;
	return shadedColor;
}

//*** Main *********************************************************************
void main(void)
{	
	//*** Texture information from G-Buffer ***
	vec4 vsPosition = texture(vsPositionTex, vert_UV);
	vec3 vsNormal = texture(vsNormalTex, vert_UV).xyz;
	vec4 materialColorD = texture(ColorTex, vert_UV);

	//*** Shading ***
	// Ambient term
	vec4 shaded = vec4(ambientColor, 1.0f);

	// Lights are shaded only with light color
	// Everything else is shaded with diffuse shading	
	vec3 lightVector;
	vec3 halfVector;
	
	//*** Lighting Jan Sobotta*** 
	//lightVector = normalize( vec4(Light.Position[0], 1.0f) - vsPosition ).xyz;
	//shaded = getLighted(lightVector, materialColorD, vsNormal);
	
	//*** Correct lighting ***
	// Perform shading for every light source
	for(int i=0; i < Light.Count; i++)
	{	
		if(materialColorD.a == 0.00 || materialColorD.a == 0.99)
		{
			shaded = vec4(materialColorD.rgb, 1.0);
		}
		else
		{
			lightVector = normalize( (ViewMatrix *  vec4(Light.Position[i], 1.0f)) - vsPosition ).xyz;
			vec3 lightDiffuse = Light.Diffuse[i].rgb;
			vec3 lightSpecular = Light.Specular.rgb;
			shaded += 1.0/Light.Count * vec4( phongShading(lightVector, vsNormal, materialColorD.rgb, lightDiffuse, lightSpecular , Shininess), 1.0f );
		}
	}
	FragColor = shaded;
}