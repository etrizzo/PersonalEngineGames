#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 4) uniform sampler2D gTexDisplacement;

uniform float TEXEL_SIZE_X;
uniform float TEXEL_SIZE_Y;

in vec4 passColor;
in vec2 passUV; 

out vec4 outColor; 

// Declare the uniform block, stating I expect it to be at
// slot 0, with memory layout following std140
layout(binding=8, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 
  
void main() 
{
	float scaledTime = GAME_TIME * .05;
	vec2 dispUVs = vec2(passUV.x + scaledTime, passUV.y + scaledTime);
 	vec4 displacement = texture(gTexDisplacement, dispUVs);

	vec2 newUVs = passUV;
	newUVs.x += (displacement.r * TEXEL_SIZE_X * 20.0);
	newUVs.y += (displacement.g * TEXEL_SIZE_Y * 20.0);
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, newUVs ); 
  
   outColor = (diffuse * passColor) ;
}