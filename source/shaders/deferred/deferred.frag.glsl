#version 400

in vec2 vert_UV;

out vec4 FragColor;

struct LightInfo{
	vec4 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
uniform LightInfo Light;
uniform float Shininess;
uniform int textureID;
uniform sampler2D deferredPositionTex;
uniform sampler2D deferredColorTex;
uniform sampler2D deferredNormalTex;
uniform sampler2D deferredDepthTex;


vec3 diffuseShading(vec3 position, vec3 normal, vec3 diffuseColor)
{
	vec3 lightVector = normalize(vec3(Light.Position) - position);
	float cosinus = max(dot(lightVector, normal), 0.0f);
	vec3 shaded = Light.Ambient + Light.Diffuse * diffuseColor * cosinus + Light.Specular * pow(cosinus, Shininess);
	return shaded;
}

void main(void) {
	if(textureID == -1){
		vec3 pos = vec3(texture(deferredPositionTex, vert_UV));
		vec3 normal = vec3(texture(deferredNormalTex, vert_UV));
		vec3 diffuseColor = vec3(texture(deferredColorTex, vert_UV));
		FragColor = vec4(diffuseShading(pos, normal, diffuseColor), 1.0f);
	}
	else if(textureID == 0)
		FragColor = texture(deferredPositionTex, vert_UV);
	else if(textureID == 1)
		FragColor = texture(deferredColorTex, vert_UV);
	else if(textureID == 2)
		FragColor = texture(deferredNormalTex, vert_UV);
	else if(textureID == 3)
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) - texture(deferredDepthTex, vert_UV);
		
}