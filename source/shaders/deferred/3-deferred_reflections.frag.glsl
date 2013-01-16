#version 400

// INS
in vec2 vert_UV;

// OUTS
out vec4 FragColor;

// UNIFORMS
uniform sampler2D diffuseColorTex;

// MAIN
void main(void)
{	
	FragColor = texture(diffuseColorTex, vert_UV);
}