#version 400

/*** Uniform block definitions ************************************************/

/*** Input ********************************************************************/
in vec2 vert_UV;

/*** Output *******************************************************************/
out vec4 FragColor;

/*** Uniforms *****************************************************************/
uniform int textureID;

uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredMaterialIDTex;
uniform sampler2D deferredReflectanceTex;
uniform sampler2D deferredReflecVecTex;
uniform sampler2D deferredDepthTex;

uniform sampler2D deferredDiffuseTex;

uniform sampler2D deferredSSRTex;

/*** Functions ****************************************************************/

/*** Main *********************************************************************/
void main(void)
{
	if(textureID == -1)
	{
		vec4 diffuse = texture(deferredDiffuseTex, vert_UV);
		vec4 reflections = texture(deferredSSRTex, vert_UV);

		FragColor = diffuse + reflections;

	}
	// Positions
	else if(textureID == 0)
		FragColor = texture(deferredPositionTex, vert_UV);
	// Albedo (Color)
	else if(textureID == 1)
		FragColor = texture(deferredColorTex, vert_UV);
	// Normals
	else if(textureID == 2)
		FragColor = texture(deferredNormalTex, vert_UV);
	// Material IDs
	else if(textureID == 3)
		FragColor = texture(deferredMaterialIDTex, vert_UV);
	// Reflectance
	else if(textureID == 4)
	{
		FragColor = vec4( texture(deferredReflectanceTex, vert_UV).a );
	}
	// Reflection vector
	else if(textureID == 5)
	{
		FragColor = texture(deferredReflecVecTex, vert_UV);
	}
	// Depth
	else if(textureID == 6)
	{
		FragColor = vec4( float(texture(deferredDepthTex, vert_UV) ) );
	}
}