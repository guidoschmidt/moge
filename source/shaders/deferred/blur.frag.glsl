//FRAGMENT SHADER
#version 400

//*** Uniform block definitions ************************************************

//*** Input ********************************************************************
in vec2 vert_UV;

//*** Output *******************************************************************
layout (location = 0) out vec4 FragColor;

//*** Uniforms *****************************************************************
uniform sampler2D DiffuseTex;


vec4 FastGaussianBlurY(in sampler2D texture, in vec2 texCoord)
{
	float blurSize = 1.0/(800.0);
	vec4 sum = vec4(0.0);
 	
 	int lod = 1;

   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture2D(texture, vec2(texCoord.x - blurSize, texCoord.y)) * 0.05;
   sum += texture2D(texture, vec2(texCoord.x - blurSize, texCoord.y)) * 0.09;
   sum += texture2D(texture, vec2(texCoord.x - blurSize, texCoord.y)) * 0.12;
   sum += texture2D(texture, vec2(texCoord.x - blurSize, texCoord.y)) * 0.15;
   sum += texture2D(texture, vec2(texCoord.x, texCoord.y)) * 0.16;
   sum += texture2D(texture, vec2(texCoord.x + blurSize, texCoord.y)) * 0.15;
   sum += texture2D(texture, vec2(texCoord.x + blurSize, texCoord.y)) * 0.12;
   sum += texture2D(texture, vec2(texCoord.x + blurSize, texCoord.y)) * 0.09;
   sum += texture2D(texture, vec2(texCoord.x + blurSize, texCoord.y)) * 0.05;
 
   return sum;
}

//*** Main *********************************************************************
void main(void)
{
	vec4 color = FastGaussianBlurY(DiffuseTex, vert_UV);
	FragColor = color;
}