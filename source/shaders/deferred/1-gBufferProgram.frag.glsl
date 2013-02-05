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
in vec3 vert_EyePosition;

/*** Output *******************************************************************/
layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Color;
layout (location = 2) out vec3 Normal;
layout (location = 3) out vec3 MaterialIDs;
layout (location = 4) out vec4 Reflectance;

/*** Uniforms *****************************************************************/
uniform bool useNormalMapping;

uniform MaterialInfo Material;

uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform samplerCube cubeMapTex;

/*** Functions ****************************************************************/
// @source: http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}
// @source: http://www.geeks3d.com/20130122/normal-mapping-without-precomputed-tangent-space-vectors/
vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 map = texture(normalTex, vert_UV).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, V, vert_UV);
    return normalize(TBN * map);
}

/*** Main *********************************************************************/
void main(void)
{
	// G-Buffer: Position
	Position = vert_Position;
	// G-Buffer: Color/Albedo
	Color = texture(colorTex, vert_UV).rgb;
	// G-Buffer: Normal
	Normal = normalize(vert_Normal);

	// Normal mapping
	vec3 normalmap = texture(normalTex, vert_UV).rgb;
	if(useNormalMapping)
	{
		if(normalmap.r != 0 && normalmap.g != 0 && normalmap.b != 0){
			vec3 N = normalize(vert_Normal.xyz);
		  	vec3 V = normalize(vert_EyePosition.xyz);
		  	vec3 PN = perturb_normal(N, V, vert_UV);
			Normal = PN;
		}
	}
	
	// G-Buffer: Reflections
	vec3 cubeMapColor = texture(cubeMapTex, vert_ReflectDirection.xyz).rgb;
	Reflectance.rgb = cubeMapColor.rgb;
	//Reflectance.a = texture(colorTex, vert_UV).a;
	
	if(normalmap.r != 0 && normalmap.g != 0 && normalmap.b != 0){
		Reflectance.a = texture(colorTex, vert_UV).a;
	}
	
	switch(Material.id)
	{
		case -1: // -1 is light
			MaterialIDs.r = 0.0f;
			MaterialIDs.g = 0.0f;
			MaterialIDs.b = 0.0f;
			Reflectance.a = 0.0f;
			break;
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
		default:
			MaterialIDs.r = 1.0f;
			MaterialIDs.g = 1.0f;
			MaterialIDs.b = 1.0f;
			break;
	}
}