//FRAGMENT SHADER
#version 400

//*** Uniform block definitions ************************************************
struct ScreenInfo
{
	float Width;
	float Height;
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
out vec4 FragColor;

//*** Uniforms *****************************************************************
uniform ScreenInfo Screen;
uniform CameraInfo Camera;

uniform int textureID;
uniform bool blurSwitch;

uniform sampler2D wsPositionTex;
uniform sampler2D vsPositionTex;
uniform sampler2D ColorTex;
uniform sampler2D wsNormalTex;
uniform sampler2D vsNormalTex;
uniform sampler2D ReflectanceTex;
uniform sampler2D ReflecVecTex;
uniform sampler2D EyeVecTex;
uniform sampler2D DepthTex;
uniform sampler2DMS DepthMSTex;
uniform sampler2D LinearDepthTex;
uniform sampler2D DiffuseTex;
uniform sampler2D SSRTex;
//uniform sampler2D BBTex;

uniform float kernelX;
uniform float kernelY;

vec2 kernelSize = vec2(0.0, 0.0);

//*** Functions ****************************************************************
// Linearizes a depth value
float linearizeDepth(float depth)
{
	return (2.0f * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

// Fast Gaussian blur in y-axis
vec4 FastGaussianBlurY(in sampler2D texture)
{
	float blurSize = 1.0/(Screen.Height * 1.0/kernelY);
	vec4 sum = vec4(0.0);
 	
 	int lod = 4;

   sum += textureLod(texture, vec2(vert_UV.x - 4.0 * blurSize, vert_UV.y), lod) * 0.05;
   sum += textureLod(texture, vec2(vert_UV.x - 3.0 * blurSize, vert_UV.y), lod) * 0.09;
   sum += textureLod(texture, vec2(vert_UV.x - 2.0 * blurSize, vert_UV.y), lod) * 0.12;
   sum += textureLod(texture, vec2(vert_UV.x - blurSize, vert_UV.y), lod) * 0.15;
   sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y), lod) * 0.16;
   sum += textureLod(texture, vec2(vert_UV.x + blurSize, vert_UV.y), lod) * 0.15;
   sum += textureLod(texture, vec2(vert_UV.x + 2.0 * blurSize, vert_UV.y), lod) * 0.12;
   sum += textureLod(texture, vec2(vert_UV.x + 3.0 * blurSize, vert_UV.y), lod) * 0.09;
   sum += textureLod(texture, vec2(vert_UV.x + 4.0 * blurSize, vert_UV.y), lod) * 0.05;
 
   return sum;
}

// Fast Gaussian blur in x-axis
vec4 FastGaussianBlurX(in sampler2D texture)
{
	float blurSize = 1.0/(Screen.Width * 1.0/kernelX);

	vec4 sum = vec4(0.0);

 	int lod = 4;

	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y - 4.0*blurSize), lod) * 0.05;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y - 3.0*blurSize), lod) * 0.09;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y - 2.0*blurSize), lod) * 0.12;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y - blurSize), lod) * 0.15;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y), lod) * 0.16;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y + blurSize), lod) * 0.15;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y + 2.0*blurSize), lod) * 0.12;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y + 3.0*blurSize), lod) * 0.09;
	sum += textureLod(texture, vec2(vert_UV.x, vert_UV.y + 4.0*blurSize), lod) * 0.05;
 
   return sum;
}

//*** Main *********************************************************************
void main(void)
{
	vec4 diffuse = texture(DiffuseTex, vert_UV);
	vec4 reflections = texture(SSRTex, vert_UV);
	vec4 EnvMapColor = texture(ReflectanceTex, vert_UV);
	float Reflectance = texture(ReflectanceTex, vert_UV).a;

	if(textureID == -1)
	{
		if(blurSwitch)
		{
			vec4 blurredSSRX = FastGaussianBlurX(SSRTex);
			vec4 blurredSSRY = FastGaussianBlurY(SSRTex);
			vec4 blurredSSR = mix(blurredSSRX, blurredSSRY, 0.5);
			
			vec4 blurredEnvMapColorX = FastGaussianBlurX(ReflectanceTex);
			vec4 blurredEnvMapColorY = FastGaussianBlurY(ReflectanceTex);
			vec4 blurredEnvMapColor = mix(blurredEnvMapColorX, blurredEnvMapColorY, 0.5);
			
			vec4 blurredReflections = blurredSSR + Reflectance * blurredEnvMapColor;
			FragColor = diffuse + blurredReflections;
		}
		else
		{
			FragColor = diffuse + (reflections + Reflectance * EnvMapColor);
		}
	}
	// World space positions
	else if(textureID == 0)
		FragColor = texture(wsPositionTex, vert_UV);
	// View space positions
	else if(textureID == 1)
		FragColor = texture(vsPositionTex, vert_UV);
	// Albedo (color)
	else if(textureID == 2)
		FragColor = texture(ColorTex, vert_UV);
	// Material IDs
	else if(textureID == 3)
	{
		float id = texture(ColorTex, vert_UV).a;
		if(id == 0.00)
			FragColor = vec4(1.0);
		else if(id <= 0.01)
			FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		else if(id <= 0.02)
			FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		else if(id <= 0.03)
			FragColor = vec4(0.0, 0.0, 1.0, 1.0);
		else if(id <= 0.04)
			FragColor = vec4(1.0, 1.0, 0.0, 1.0);
		else if(id <= 0.05)
			FragColor = vec4(1.0, 0.0, 1.0, 1.0);
		else if(id <= 0.06)
			FragColor = vec4(0.0, 1.0, 1.0, 1.0);
		else if(id <= 0.07)
			FragColor = vec4(1.0, 0.5, 0.5, 1.0);
		else if(id <= 0.08)
			FragColor = vec4(0.0, 0.5, 1.0, 1.0);
		else if(id <= 0.09)
			FragColor = vec4(1.0, 0.0, 0.5, 1.0);
		else if(id <= 0.10)
			FragColor = vec4(0.25, 0.5, 0.25, 1.0);
	}
	// World space normals
	else if(textureID == 4)
		FragColor = texture(wsNormalTex, vert_UV);
	// View space normals
	else if(textureID == 5)
		FragColor = texture(vsNormalTex, vert_UV);
	// Environment mapping
	else if(textureID == 6)
	{
		FragColor = Reflectance * EnvMapColor;
	}
	// Reflectance
	else if(textureID == 7)
	{
		FragColor = vec4( texture(ReflectanceTex, vert_UV).a );
	}
	// Reflection vector
	else if(textureID == 8)
	{
		FragColor = texture(ReflecVecTex, vert_UV);
	}
	// Depth
	else if(textureID == 9)
	{
		float depth = linearizeDepth( float(texture(DepthTex, vert_UV) ) ); 
		FragColor = vec4(depth, depth, depth, 1.0f);
	}
	// Linear Depth
	else if(textureID == 10)
	{
		//vec4 color = texelFetch(DepthMSTex, vert_UV, 0);
		//FragColor = color;
	}
	// SSR Pass
	else if(textureID == 11)
	{
		FragColor = vec4( texture(SSRTex, vert_UV).rgb, 1.0f );
	}
	// BB Pass
	/*
	else if(textureID == 12)
	{
		FragColor = vec4( texture(BBTex, vert_UV).rgb, 1.0f );
	}
	*/
}