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
	mat4 ModelMatrix[3];
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

//*** Output *******************************************************************
//layout (location = 0) out vec3 wsPosition;
layout (location = 0) out vec3 vsPosition;
layout (location = 1) out vec3 vsNormal;
layout (location = 2) out vec4 Color;
//layout (location = 3) out vec3 wsNormal;
layout (location = 3) out vec4 Reflectance;
layout (location = 4) out vec4 Billboards;
//layout (location = 5) out vec4 Free;
// Location 8 is DepthBuffer

//*** Uniforms *****************************************************************
uniform CameraInfo Camera;
uniform ImpostorInfo Impostor;
uniform AABBInfo AABB;
uniform MaterialInfo Material;

uniform int billboardCount;
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

uniform sampler2D ColorTex;
uniform sampler2D ImpostorTex[3];
uniform sampler2D NormalTex;
uniform samplerCube CubeMapTex;

//*** Functions ****************************************************************
// Simple environment mapping
vec3 CubeMapping(in vec3 wsReflectionVector)
{
	vec3 reflectedColor = texture(CubeMapTex, wsReflectionVector).rgb;
	
	return reflectedColor;
}

//	Parallax corrected cube mapping
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

// Intersect a triangle with a given ray
vec3 IntersectTriangle(in vec3 rayOrigin, in vec3 rayDirect, in vec3 vert0, in vec3 vert1, in vec3 vert2)
{
	float t, u, v;

	vec3 edge1, edge2, edge3;
	float det, inv_det;

	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;

	vec3 pvec = cross(rayDirect, edge2);
	det = dot(edge1, pvec);

	inv_det = 1.0 / det;
	vec3 tvec = rayOrigin - vert0;
	u = dot(tvec, pvec) * inv_det;

	vec3 qvec = cross(tvec, edge1);
	v = dot(rayDirect, qvec) * inv_det;

	t = dot(edge2, qvec) * inv_det;

	return vec3(u, v, t);
}

// Fresnel function
float fresnel(vec3 reflection, vec3 normal, float R0) {
    // float const R0 =  pow(1.0 - refractionIndexRatio, 2.0) /
    //                   pow(1.0 + refractionIndexRatio, 2.0);
    // reflection and normal are assumed to be normalized
    return R0 + (1.0 - R0) * pow(1.0 - dot(reflection, normal), 5.0);
}


vec4 FastGaussianBlurY(in sampler2D texture, in vec2 texCoord)
{
	float blurSize = 1.0/(800.0);
	vec4 sum = vec4(0.0);
 	
 	int lod = 4;

   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += textureLod(texture, vec2(texCoord.x - blurSize, texCoord.y), lod) * 0.05;
   sum += textureLod(texture, vec2(texCoord.x - blurSize, texCoord.y), lod) * 0.09;
   sum += textureLod(texture, vec2(texCoord.x - blurSize, texCoord.y), lod) * 0.12;
   sum += textureLod(texture, vec2(texCoord.x - blurSize, texCoord.y), lod) * 0.15;
   sum += textureLod(texture, vec2(texCoord.x, texCoord.y), lod) * 0.16;
   sum += textureLod(texture, vec2(texCoord.x + blurSize, texCoord.y), lod) * 0.15;
   sum += textureLod(texture, vec2(texCoord.x + blurSize, texCoord.y), lod) * 0.12;
   sum += textureLod(texture, vec2(texCoord.x + blurSize, texCoord.y), lod) * 0.09;
   sum += textureLod(texture, vec2(texCoord.x + blurSize, texCoord.y), lod) * 0.05;
 
   return sum;
}


vec4 FastGaussianBlurX(in sampler2D texture, in vec2 texCoord)
{
	float blurSize = 1.0/(800.0);

	vec4 sum = vec4(0.0);

 	int lod = 4;
	
	// blur in y (vertical)
	// take nine samples, with the distance blurSize between them
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y - blurSize), lod) * 0.05;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y - blurSize), lod) * 0.09;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y - blurSize), lod) * 0.12;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y - blurSize), lod) * 0.15;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y), lod) * 0.16;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y + blurSize), lod) * 0.15;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y + blurSize), lod) * 0.12;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y + blurSize), lod) * 0.09;
	sum += textureLod(texture, vec2(texCoord.x, texCoord.y + blurSize), lod) * 0.05;
 
   return sum;
}

// Billboard reflections
vec4 BillboardReflections(in vec3 wsPosition, in vec3 wsReflectVec)
{
	vec4 reflectedColor = vec4(0.0);

	// Every Billboard
	for(int i = 0; i < billboardCount; i++)
	{
		float pixelSize = 1.0/250.0;

		//Reflectance.a = 0.0;
		vec3 vert0 = vec3( 0.5, 0.0,  0.5);
		vec3 vert1 = vec3(-0.5, 0.0, -0.5);
		vec3 vert2 = vec3(-0.5, 0.0,  0.5);

		// Build ray
		vec3 rayOrigin = wsPosition;
		vec3 rayDirect = wsReflectVec;

		// Transform billboard to world space
		vert0 = ( Impostor.ModelMatrix[i] * vec4(vert0, 1.0) ).xyz;
		vert1 = ( Impostor.ModelMatrix[i] * vec4(vert1, 1.0) ).xyz;
		vert2 = ( Impostor.ModelMatrix[i] * vec4(vert2, 1.0) ).xyz;

		// Intersect the billboard and get texture coordinates
		vec3 uv = IntersectTriangle(rayOrigin, rayDirect, vert2, vert0, vert1);

		// Check if texture coordinates are valid between 0.0 and 1.0
		if(uv.x > 0.0 && uv.x < 1.0 &&
		   uv.y > 0.0 && uv.y < 1.0)
		{
			// Check alpha channel of billboard texture
			float alpha = texture(ImpostorTex[i], uv.xy).a;
			
			vec3 color;
			color = texture(ImpostorTex[i], uv.xy).rgb;
			reflectedColor.rgb = color * alpha;
		}
	}


	return reflectedColor;
}

// Normal mapping: calculate cotangents
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

// Normal mapping: 
// @source: http://www.geeks3d.com/20130122/normal-mapping-without-precomputed-tangent-space-vectors/
vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 map = texture(NormalTex, vert_UV).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, V, vert_UV);
    return normalize(TBN * map);
}

//*** Main *********************************************************************
void main(void)
{
	//*** Filling the G-Buffer ***
	// Positions
	vec3 wsPosition = vert_wsPosition;
	vsPosition = vert_vsPosition;
	// Normals
	vsNormal = normalize(vert_vsNormal);
	vec3 wsNormal = normalize(vert_wsNormal);
	// Colors (Albedo) & Alpha blending
	float alpha = texture(ColorTex, vert_UV).a;
	if(alpha < 0.75)
		discard;
	vec3 OutputColor = texture(ColorTex, vert_UV).rgb * alpha;
	Color.a = alpha;
	Color.rgb = OutputColor;
	// Reflectance
	Reflectance.a = 0.0;
	Reflectance.a = texture(NormalTex, vert_UV).a;
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
		Reflectance.a = 0.0;
	}
	// Eye vectors
	vec3 wsEyeVec = normalize(vert_wsEyeVector); 
	vec3 vsEyeVec = normalize(vert_vsEyeVector);
	
	//*** Normal mapping ***
	vec3 normalmap = texture(NormalTex, vert_UV).rgb;
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
	vec3 vsReflectVec      = normalize( reflect(vsEyeVec, vsNormal) ); 

	//*** Environment mapping using cube maps ***
	if(toggleCM)
	{
		vec3 CubeMapColor = vec3(0.0);
		CubeMapColor = CubeMapping(wsReflectVec);
		
		//** Parallax-corrected cube mapping ***
		if(togglePCCM)
		{
			CubeMapColor = ParallaxCorrecteCubeMapping(wsPosition, wsReflectVec);
		}
		
		Reflectance.rgb = CubeMapColor;
	}

	float f = fresnel(wsReflectVec, wsNormal, 0.15); 


	//*** Billboard reflections ***
	vec4 reflectedColor = vec4(0.0);
	if(toggleBB)
	{
		reflectedColor = BillboardReflections(wsPosition, wsReflectVec);

		// Write to texture
		Billboards = reflectedColor;
	}
}