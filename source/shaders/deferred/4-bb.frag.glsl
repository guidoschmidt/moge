#version 400


in vec3 vert_Position;
in vec3 vert_Normal;
in vec2 vert_UV;

out vec3 FragColor;

uniform vec2 Screen;

uniform sampler2D colorTex; 

float intersect(in vec3 rayO, in vec3 rayD)
{
	return vert_Position.z;
}


void main()
{
	// Fragment coordinates from 0 to 1
	vec2 uv = gl_FragCoord.xy/vec2(800, 800);
	vec3 color = vec3(0.0f);

	// Create a ray with rayO origin and rayD destination
	vec3 ray = normalize( reflect( -vert_Position, vert_Normal) );
	

	FragColor = color;
}