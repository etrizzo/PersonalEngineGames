#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;

layout(binding = 4) uniform sampler2D gTexGradient;


layout(binding=8, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 

layout(binding=9, std140) uniform uboSizeBuffer
{
   float TEXEL_SIZE_X;
   float TEXEL_SIZE_Y;

   //trash really?
   float SCREEN_SIZE_X;
   float SCREEN_SIZE_Y;
};


in vec4 passColor;
in vec2 passUV;
//in vec3 normalizedWorldPos;
in vec2 worldUV;

out vec4 outColor;

void main( void )
{

	vec4 diffuse = texture( gTexDiffuse, passUV );
	vec4 gradientColor = texture( gTexGradient, worldUV );
	if (diffuse.w <= .1){
		discard;
	} 
	outColor = diffuse * passColor;

	//outColor = gradientColor;
	
}