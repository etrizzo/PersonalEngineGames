#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 4) uniform sampler2D gTexDisplacement;



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

layout(binding=9, std140) uniform uboSizeBuffer
{
   float TEXEL_SIZE_X;
   float TEXEL_SIZE_Y;

   //trash really?
   float SCREEN_SIZE_X;
   float SCREEN_SIZE_Y;
};

float AbsValue(float val)
{
   if (val < 0.0){
      return val * -1.0;
   } else {
      return val;
   }
}

bool DoColorsExceedEdgeThreshold(vec4 center, vec4 sampleColor, float threshold)
{
   if (AbsValue(center.r - sampleColor.r) > threshold)
   {
      return true;
   }

   if (AbsValue(center.g - sampleColor.g) > threshold)
   {
      return true;
   }

   if (AbsValue(center.b - sampleColor.b) > threshold)
   {
      return true;
   }

   if (AbsValue(center.a - sampleColor.a) > threshold)
   {
      return true;
   }

   return false;
}

void main() 
{
	float scaledTime = GAME_TIME * .05;
	vec2 dispUVs = vec2(passUV.x + scaledTime, passUV.y + scaledTime);
 	vec4 displacement = texture(gTexDisplacement, dispUVs);


   vec4 edgeColor = vec4 (1.0f, 1.0f, 1.0f, 1.0f);
   int halfSize = 1;
   vec4 diffuseColor = texture(gTexDiffuse, passUV);
   for(int x = (-1 * halfSize); x < halfSize; x++ )
   {
      for(int y = (-1 * halfSize); y < halfSize; y++ )
      {
         vec2 newUVs = passUV;
         newUVs.x += (x * TEXEL_SIZE_X);
         newUVs.y += (y * TEXEL_SIZE_Y);
         vec4 sampleColor = texture(gTexDiffuse, newUVs);
         if (DoColorsExceedEdgeThreshold(diffuseColor, sampleColor, 0.2)){
            edgeColor = vec4(0.0,0.0,0.0,1.0);
         }
      }
   }

   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV); 
  
   outColor = (edgeColor) ;
}