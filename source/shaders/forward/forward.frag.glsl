#version 400

in vec3 vert_Position;
in vec3 vert_Normal;
in vec2 vert_UV;

out vec4 FragColor;

uniform sampler2D colorTex;

void main(void) {
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); 
	FragColor = texture(colorTex, vert_UV);
}