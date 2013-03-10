#version 400

/*** Uniform block definitions ************************************************/
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

struct MaterialInfo
{
	float id;
	float reflectance;
};

struct AABBInfo
{
	vec3 max;
	vec3 min;
};


struct ImpostorInfo
{
	vec3 vertex;
	vec3 normal;
	vec3 uMax;
	vec3 vMax;
};

/*** Input ********************************************************************/
in vec3 vert_wsPosition;
in vec3 vert_vsPosition;
in vec3 vert_vsNormal;
in vec3 vert_wsNormal;
in vec2 vert_UV;
in vec3 vert_vsEyeVector;
in vec3 vert_wsEyeVector;
in vec3 vert_wsEyePosition;
in float vert_LinearDepth;

/*** Output *******************************************************************/
layout (location = 0) out vec3 wsPosition;
layout (location = 1) out vec3 vsPosition;
layout (location = 2) out vec4 Color;
layout (location = 3) out vec3 wsNormal;
layout (location = 4) out vec3 vsNormal;
layout (location = 5) out vec4 Reflectance;
layout (location = 6) out vec3 vsReflectVec;
layout (location = 7) out vec3 LinearDepth;
//layout (location = 8) out vec3 Depth (DepthBuffer)

/*** Uniforms *****************************************************************/
uniform CameraInfo Camera;
uniform ImpostorInfo Impostor;
uniform AABBInfo AABB;
uniform MaterialInfo Material;

uniform bool useNormalMapping;
uniform bool togglePCCM;
uniform bool toggleCM;
uniform bool toggleBB;

uniform vec3 wsCubeMapPosition;
uniform vec3 lightColor;

uniform sampler2D colorTex;
uniform sampler2D impostorTex;
uniform sampler2D normalTex;
uniform samplerCube cubeMapTex;

uniform mat4 wsNormalMatrix;
uniform mat4 vsNormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

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
	// G-Buffer: Positions, Colors (Albedo), Normals
	wsPosition = vert_wsPosition;
	vsPosition = vert_vsPosition;
	Color.rgb = texture(colorTex, vert_UV).rgb;
	// G-Buffer: Linear Depth
	LinearDepth = vec3(vert_LinearDepth);
	// Bounding boxes
	if(Material.id == 0.99)
	{
		Color.rgb = vec3(1.0, 0.0, 0.0);
		Color.a = 0.09;
	}
	// Lights
	if(Material.id == 0.00)
	{
		Color.rgb = lightColor;
	}
	Color.a = Material.id;
	vsNormal = normalize(vert_vsNormal);
	wsNormal = normalize(vert_wsNormal);

	// G-Buffer: Eye vectors
	vec3 wsEyeVec = normalize(vert_wsEyeVector); 
	vec3 vsEyeVec = normalize(vert_vsEyeVector);
	
	// Normal mapping
	vec3 normalmap = texture(normalTex, vert_UV).rgb;
	if(useNormalMapping)
	{
		if(normalmap.r != 0 && normalmap.g != 0 && normalmap.b != 0){
			vec3 vsN = vsNormal;
		  	vec3 vsV = normalize(vert_vsEyeVector.xyz);
		  	vec3 vsPN = perturb_normal(vsN, vsV, vert_UV);
			vsNormal = vsPN;
			
			vec3 wsN = wsNormal;
		  	vec3 wsV = normalize(vert_wsEyeVector.xyz);
		  	vec3 wsPN = perturb_normal(wsN, wsV, vert_UV);
			wsNormal = wsPN;
		}
	}

	// G-Buffer: Reflection vectors
	vsReflectVec      = normalize( reflect(-vert_vsEyeVector, vsNormal) ); 
	vec3 wsReflectVec = normalize( reflect(wsEyeVec, wsNormal) );

	// Parallax-corrected cube mapping
	if(togglePCCM)
	{
		vec3 firstPlaneIntersection = (AABB.max - wsPosition) / wsReflectVec;
		vec3 secondPlaneIntersection = (AABB.min - wsPosition) / wsReflectVec;

		vec3 furthestPlane = max(firstPlaneIntersection, secondPlaneIntersection);
		float distance = min( min(furthestPlane.x, furthestPlane.y), furthestPlane.z );

		vec3 wsIntersection = wsPosition + distance * wsReflectVec;
		wsReflectVec = wsIntersection - wsCubeMapPosition;
	}

	// Cube mapping reflections
	if(toggleCM)
	{
		vec3 cubeMapColor = texture(cubeMapTex, wsReflectVec).rgb;
		if(lightColor != 0.0)
			cubeMapColor *= lightColor;
		Reflectance.rgb = cubeMapColor;
	}

	// G-Buffer: Reflectance
	Reflectance.a = texture(colorTex, vert_UV).a;
	// Bounding boxes
	if(Material.id == 0.09)
	{
		Reflectance.a = 0.0;
	}

	// Billboard stuff
	vec3 color;
	if(toggleBB)
	{
		float orthogonality = dot(-wsReflectVec, vec3(0.0, 0.0, 1.0));

		if(orthogonality > 0.01)
		{
			vec3 vector = Impostor.vertex - wsPosition;
			float distance = -dot(vector, Impostor.normal);
			
			vec3 intersection = wsPosition + normalize(wsReflectVec);

			vec3 uAxis = Impostor.uMax - Impostor.vertex.x;
			vec3 vAxis = Impostor.vMax - Impostor.vertex.y;

			if(intersection.x > Impostor.vertex.x)
			{
				vec2 texcoord;
				texcoord.x = intersection.x * uAxis.x * -0.01;
				texcoord.y = intersection.y * vAxis.y * -0.6;
			
				color = texture(impostorTex, texcoord).rgb;
			}
		}
	}

	Color.rgb += color;
}