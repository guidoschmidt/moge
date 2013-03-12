#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

struct ImpostorInfo
{
	vec3 position;
	vec3 normal;
	mat4 ModelMatrix;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
};

/*** Input ********************************************************************/
in vec2 vert_UV;

/*** Output *******************************************************************/
layout (location  = 0) out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform ImpostorInfo Impostors[2];
uniform CameraInfo Camera;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform sampler2D wsPositionTex;
uniform sampler2D wsNormalTex;
uniform sampler2D ReflectanceTex;
uniform sampler2D DepthTex;
uniform sampler2D DiffuseTex;

uniform sampler2D ImpostorTextures[2];

/*** Functions ****************************************************************/
/*
 * Linearizes a depth value
 */
float linearizeDepth(float depth)
{
	return (2.0f * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

float getCoord(in float i1,in float i2, in float w1, in float w2, in float p)
{
    return ((p - i1) / (i2 - i1)) * (w2 - w1) + w1;
}

float IntersectImpostor(in vec3 wsPosition, in vec3 position, in vec3 normal)
{
	vec3 wsPosToImposPosition = position - wsPosition;
	float distance = -dot(wsPosToImposPosition, normal);
	return distance;
}

/*** Main *********************************************************************/
void main(void)
{
	vec3 wsPosition = texture(wsPositionTex, vert_UV).xyz;
	vec3 wsNormal = normalize( texture(wsNormalTex, vert_UV).xyz );
	float Depth = linearizeDepth( texture(DepthTex, vert_UV).z );

	vec3 wsEyeVector = wsPosition - Camera.Position;
	vec3 wsReflectionVector = normalize( reflect(normalize(wsEyeVector), wsNormal) );

	vec4 color = vec4(0.0);
	
	vec3 wsImpostorPosition = Impostors[0].position;
	vec3 wsImpostorNormal   = Impostors[0].normal.xyz;
	// Right upper corner
	vec3 wsImpostorRUC 		= (ModelMatrix * Impostors[0].ModelMatrix * vec4(Impostors[0].position - vec3(1.0, 0.0, 0.0), 1.0)).xyz;
	// Left lower corner
	vec3 wsImpostorLLC 		= (ModelMatrix * Impostors[0].ModelMatrix * vec4(Impostors[0].position - vec3(0.0, -1.0, 0.0), 1.0)).xyz;

	// Intersect with Impostor
	vec3 vector = wsImpostorPosition - wsPosition;
	float distance = -dot(vector, vec3(0, 0, -1));
	vec3 intersection = wsPosition + distance * normalize(wsReflectionVector);

	vec2 texCoord = vec2(intersection.x, intersection.y) ;
	if(texCoord.x >= 0.0 && texCoord.x <= 1.0 &&
	   texCoord.y >= 0.0 && texCoord.y <= 1.0)
	{
		//color = texture(ImpostorTextures[0], texCoord);
	}

	vec4 diffuseColor = texture(DiffuseTex, vert_UV);

	FragColor = diffuseColor + color;
}