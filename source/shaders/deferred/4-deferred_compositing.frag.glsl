#version 400

/*** Uniform block definitions ************************************************/
struct ScreenInfo
{
	float Width;
	float Height;
};

/*** Input ********************************************************************/
in vec2 vert_UV;

/*** Output *******************************************************************/
out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform ScreenInfo Screen;

uniform int textureID;

uniform bool blurSwitch;

uniform sampler2D deferredWorldPositionTex;
uniform sampler2D deferredViewPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredReflecVecTex;
uniform sampler2D deferredDepthTex;

uniform sampler2D deferredDiffuseTex;

uniform sampler2D deferredSSRTex;

uniform int kernelX;
uniform int kernelY;

float kernel[13];
ivec2 kernelSize = ivec2(0, 0);

/*** Functions ****************************************************************/

/* 
 *
 */
vec3 blur(in vec2 uv)
{
	kernelSize.x = kernelX;
	kernelSize.y = kernelY;

	kernel[0] = kernel[12] = 0.5000f;
	kernel[1] = kernel[11] = 0.6179f;
	kernel[2] = kernel[10] = 0.7257f;
	kernel[3] = kernel[ 9] = 0.8413f;
	kernel[4] = kernel[ 8] = 0.9773f;
	kernel[5] = kernel[ 7] = 0.9918f;
	kernel[6] = 0.9965f;

	vec2 pixelSize = 1.0f/vec2(Screen.Width, Screen.Height);
	vec3 color = vec3(0.0f);
	float count = 0;
	vec2 pos = uv;

	color = texture(deferredDiffuseTex, uv).rgb;
	for(int x = -kernelSize.x; x <= kernelSize.x; x++)
	{
		for(int y = kernelSize.y; y <= 2 * kernelSize.y; y++)
		{
			pos = uv + ( vec2(x, y) * pixelSize );
			color += texture(deferredSSRTex, pos).rgb * kernel[x + 6];
			count += kernel[x + 6];
		}
	}

	color /= count;

	return color;
}

/*** Main *********************************************************************/
void main(void)
{
	if(textureID == -1)
	{
		vec4 diffuse = texture(deferredDiffuseTex, vert_UV);
		vec4 reflections = texture(deferredSSRTex, vert_UV);

		if(blurSwitch)
		{
			vec4 blurredReflections = vec4(blur(vert_UV), 1.0f);
			FragColor = diffuse + (reflections.a * blurredReflections);
		}
		else
		{
			FragColor = diffuse + (reflections.a * reflections);
		}
	}
	// World Positions
	else if(textureID == 0)
		FragColor = texture(deferredWorldPositionTex, vert_UV);
	// View Positions
	else if(textureID == 1)
		FragColor = texture(deferredViewPositionTex, vert_UV);
	// Albedo (Color)
	else if(textureID == 2)
		FragColor = texture(deferredColorTex, vert_UV);
	// Normals
	else if(textureID == 3)
		FragColor = texture(deferredNormalTex, vert_UV);
	// Material IDs
	else if(textureID == 4)
		FragColor = texture(deferredMaterialIDTex, vert_UV);
	// Reflectance
	else if(textureID == 5)
	{
		FragColor = vec4( texture(deferredReflectanceTex, vert_UV).a );
		//FragColor = texture(deferredReflectanceTex, vert_UV);
	}
	// Reflection vector
	else if(textureID == 6)
	{
		FragColor = texture(deferredReflecVecTex, vert_UV);
	}
	// Depth
	else if(textureID == 7)
	{
		FragColor = vec4( float(texture(deferredDepthTex, vert_UV) ) );
	}
	// Depth
	else if(textureID == 8)
	{
		FragColor = vec4( texture(deferredSSRTex, vert_UV).rgb, 1.0f );
	}
}