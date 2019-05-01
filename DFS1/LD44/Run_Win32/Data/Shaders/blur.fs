#version 420 core
#include "Includes/mathutils.glsl"

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexDepth;

in vec4 passColor;
in vec2 passUV; 
in vec3 passViewPos;
in mat4 passView;

out vec4 outColor; 

// Declare the uniform block, stating I expect it to be at
// slot 0, with memory layout following std140

layout(binding=0, std140) uniform uFogBlock 
{
   // Fog
   vec4 FOG_COLOR; 
   float FOG_NEAR_PLANE;
   float FOG_NEAR_FACTOR;
   float FOG_FAR_PLANE;
   float FOG_FAR_FACTOR;
};


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

vec2 GetPerpendicular(vec2 a)
{
  return a.yx * vec2(1,-1);
}

vec4 GetColor(vec2 uvs)
{
   return texture(gTexDiffuse, uvs);
}


void main() 
{
   //vec4 edgeColor = GetEdgeColor(passUV);
   

   // sample (gather) our texel colour for this UV
   vec4 diffuse = GetColor(passUV);

   float depth = texture(gTexDepth, passUV).r;   //NEED TO BIND DEPTH BUFFER
   //depth *= depth;

   vec3 position = 


   //player is at 0, camera is at -10
   // camera depth is 120
   //so the focal point should be at 1/12
   // .08 ish
   depth -= .5f;
   depth = AbsValue(depth);   //need to relinearize this shit
   float sampleNumFloat = RangeMap(depth, 0.0f, 1.0f, 0.0f, 10.0f);   //change based on depth 0-5ish
   //sampleNumFloat = AbsValue(sampleNumFloat);
   //sampleNumFloat = clamp(sampleNumFloat, 0.0f, 6.0f);
    //float sampleNumFloat = smoothstep( 0.0f, 10.0f, depth ); 

   int sampleNum = int(sampleNumFloat);
   vec2 texelSize = vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);

   vec4 cumulativeColor = diffuse;
   int numSamples = 1;

   for (int x = -sampleNum; x < sampleNum; x++)
   {
       for (int y = -sampleNum; y < sampleNum; y++)
      {
          numSamples++;
          vec2 newUV = passUV + vec2(x * TEXEL_SIZE_X * 1.0f, y * TEXEL_SIZE_Y* 1.0f);

          newUV.x = clamp(newUV.x, 0.0f, 1.0f);
          newUV.y = clamp(newUV.y, 0.0f, 1.0f);
          vec4 sampleColor = GetColor(newUV);
          cumulativeColor+=sampleColor;
      }
   }
   cumulativeColor/=numSamples;

   outColor = cumulativeColor ;
   outColor.a = 1.0;
 }