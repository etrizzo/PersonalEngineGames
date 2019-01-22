#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec4 passColor;

out vec4 outColor;
// Declare the uniform block, stating I expect it to be at
// slot 0, with memory layout following std140
layout(binding=1, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 

void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );
	//vec4 newColor = vec4(passColor.g, passColor.r, passColor.b, passColor.a);
	//outColor = diffuse * newColor;
	outColor = vec4(diffuse.g,diffuse.r, diffuse.b, 1.0) * passColor;
	//outColor = vec4(passUV.xy, 0.0, 1.0) * passColor;
}