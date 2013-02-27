#version 400
struct Impostor
{
	vec3 max;
	vec3 min;
	vec3 normal;
};


/*** Input ********************************************************************/
in vec2 vert_UV;
in vec3 vert_Position;
in vec3 vert_Normal;
in vec3 vert_EyePosition;

/*** Output *******************************************************************/
out vec3 FragColor;

/*** Uniforms *****************************************************************/
uniform vec2 Screen;
uniform sampler2D colorTex;
uniform 

/*** Main *********************************************************************/
void main()
{
	vec3 EyeVec = vert_Position - vert_EyePosition;
	vec3 ReflectVec = normalize( reflect( EyeVec, vert_Normal ) );
	FragColor = ReflectVec;

	vec3 finalColor = vec3(0.0f);

	FragColor = texture(colorTex, vert_UV).rgb;
}