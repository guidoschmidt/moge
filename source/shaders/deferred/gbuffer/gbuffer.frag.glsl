//FRAGMENT SHADER
#version 400

//*** Uniform block definitions ************************************************
// Camera informations
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

// Material informations
struct MaterialInfo
{
	float id;
	float reflectance;
};

// Scene proxy geometry for parallax-corrected cube mapping
struct AABBInfo
{
	vec3 max;
	vec3 min;
};

//
struct ImpostorInfo
{
	vec3 vertex[4];
	vec3 normal;
	vec3 uMax;
	vec3 vMax;
};

//*** Input ********************************************************************
in vec3 vert_wsPosition;
in vec3 vert_wsNormal;
in vec3 vert_vsPosition;
in vec3 vert_vsNormal;
in vec2 vert_UV;
in vec3 vert_vsEyeVector;
in vec3 vert_wsEyePosition;
in vec3 vert_wsEyeVector;
in float vert_LinearDepth;

//*** Output *******************************************************************
layout (location = 0) out vec3 wsPosition;
layout (location = 1) out vec3 vsPosition;
layout (location = 2) out vec4 Color;
layout (location = 3) out vec3 wsNormal;
layout (location = 4) out vec3 vsNormal;
layout (location = 5) out vec4 Reflectance;
layout (location = 6) out vec3 vsReflectVec;
layout (location = 7) out vec3 LinearDepth;
// Location 8 is DepthBuffer

//*** Uniforms *****************************************************************
uniform CameraInfo Camera;
uniform ImpostorInfo Impostor;
uniform AABBInfo AABB;
uniform MaterialInfo Material;

uniform bool useNormalMapping;
uniform bool toggleCM;
uniform bool togglePCCM;
uniform bool toggleBB;
uniform vec3 lightColor;
uniform vec3 wsCubeMapPosition;
uniform mat4 wsNormalMatrix;
uniform mat4 vsNormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

uniform sampler2D colorTex;
uniform sampler2D impostorTex;
uniform sampler2D normalTex;
uniform samplerCube CubeMapTex;

//*** Functions ****************************************************************
// Simple environment mapping
vec3 CubeMapping(in vec3 wsReflectionVector)
{
	vec3 reflectedColor = texture(CubeMapTex, wsReflectionVector).rgb;
	
	return reflectedColor;
}

//	Parallax corrected cubemapping
vec3 ParallaxCorrecteCubeMapping(in vec3 wsPosition, in vec3 wsReflectionVector)
{
	vec3 firstPlaneIntersection  = (AABB.max - wsPosition) / wsReflectionVector;
	vec3 secondPlaneIntersection = (AABB.min - wsPosition) / wsReflectionVector;

	vec3 furthesPlane = max(firstPlaneIntersection, secondPlaneIntersection);
	float distance = min( min(furthesPlane.x, furthesPlane.y), furthesPlane.z );

	vec3 wsIntersection = wsPosition + distance * wsReflectionVector;
	vec3 wsParallaxCorrectedReflectionVector = wsIntersection - wsCubeMapPosition;
	vec3 reflectedColor = CubeMapping(wsParallaxCorrectedReflectionVector);
	
	return reflectedColor; 
}

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

//*** Main *********************************************************************
void main(void)
{
	//*** Filling the G-Buffer ***
	// Positions
	wsPosition = vert_wsPosition;
	vsPosition = vert_vsPosition;
	// Normals
	vsNormal = normalize(vert_vsNormal);
	wsNormal = normalize(vert_wsNormal);
	// Colors (Albedo) 
	Color.rgb = texture(colorTex, vert_UV).rgb;
	// Linear Depth
	LinearDepth = vec3(vert_LinearDepth);
	// Reflectance
	Reflectance.a = texture(colorTex, vert_UV).a;
	if(Material.id == 0.09)
	{
		// Bounding boxes
		Reflectance.a = 0.0;
	}
	// Materials
	Color.a = Material.id;
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
	// Eye vectors
	vec3 wsEyeVec = normalize(vert_wsEyeVector); 
	vec3 vsEyeVec = normalize(vert_vsEyeVector);
	

	//*** Normal mapping ***
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

	// Reflection vectors
	vec3 wsReflectVec = normalize( reflect(wsEyeVec, wsNormal) );
	vsReflectVec      = normalize( reflect(vsEyeVec, vsNormal) ); 

	//*** Environment mapping using cube maps ***
	if(toggleCM)
	{
		vec3 CubeMapColor = vec3(0.0);
		CubeMapColor = CubeMapping(wsReflectVec);
		Reflectance.rgb = CubeMapColor;
		
		if(togglePCCM)
		{
			/*** Parallax-corrected cube mapping ***/
			CubeMapColor = ParallaxCorrecteCubeMapping(wsPosition, wsReflectVec);
			Reflectance.rgb = CubeMapColor;
		}
	}


	//*** Billboard stuff ***
	vec3 color;
	if(toggleBB)
	{
		color = vec3(0.0, 0.0, 0.0);

		float orthogonality = abs(dot(-wsReflectVec, vec3(0.0, 0.0, -1.0)));

		// if less than this, the ray is nearly or entirely parallel to the plane
		if(orthogonality > 0.001)
		{
			color.x = 0.0;
			
			vec3 uAxis = Impostor.uMax - Impostor.vertex[1].x;
			vec3 vAxis = Impostor.vMax - Impostor.vertex[1].y;

			vec3 vector = Impostor.vertex[1] - wsPosition;
			float distance = -dot(vector, Impostor.normal);
			
			vec3 intersection = wsPosition + distance * normalize(wsReflectVec);

			if(intersection.x > Impostor.vertex[1].x && intersection.x < Impostor.vertex[1].x + Impostor.uMax.x &&
			   intersection.y > Impostor.vertex[1].y && intersection.y < Impostor.vertex[1].y - Impostor.vMax.y &&
			   intersection.z > -distance)
			{
				vec2 texcoord;
				texcoord.x = dot(intersection.x, uAxis.x) * 0.025 - 0.2;
				texcoord.y = dot(intersection.y, vAxis.y) * -0.025 - 0.08;
				color = texture(impostorTex, texcoord).rgb;
			}						
		}
	}

	Color.rgb += color;
}