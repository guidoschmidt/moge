#version 400
struct CameraInfo
{
	vec3 Position;
	vec3 LookAt;
	float NearPlane;
	float FarPlane;
};

in vec2 vert_UV;

out vec4 FragColor;

uniform CameraInfo Camera;

uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2DMS deferredDepthTex;
uniform sampler2D deferredDiffuseTex;

/*** Functions ****************************************************************/ 
float linearizeDepth(float depth)
{
	return (2.0 * Camera.NearPlane) / (Camera.FarPlane + Camera.NearPlane - depth * (Camera.FarPlane - Camera.NearPlane));
}

void main()
{
	vec4 a = texelFetch(deferredDepthTex, vert_UV, 0);
	FragColor = a;
}