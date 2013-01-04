#version 400

in vec3 vert_Position;
in vec3 vert_Normal;
in vec2 vert_UV;
in vec3 vert_Camera;

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Color;
layout (location = 2) out vec3 Normal;

uniform sampler2D colorTex;

void main(void) {
	Position = vert_Position;
	Color = vec3(texture(colorTex, vert_UV));
	Normal = vert_Normal;
}