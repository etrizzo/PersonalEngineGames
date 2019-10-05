#version 420 core
#include "Includes/mathutils.glsl"

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 6) uniform sampler2D gTexDepth;

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

   //https://stackoverflow.com/questions/50209415/sampling-the-depth-buffer-and-normalizing-to-0-1-directx
  float lineardepth = (2.0f * 0.1f) / (120.0f + 0.1f - depth * (120.0f - 0.1f));


   //player is at 0, camera is at -10
   // camera depth is 120
   //so the focal point should be at 1/12
   // .08 ish
   float blurDepth = lineardepth - .15f;
   if (blurDepth < 0.0f){
   	   blurDepth *=2.0f;
   }
   blurDepth = AbsValue(blurDepth);   //need to relinearize this shit

   blurDepth = SmoothStop2(blurDepth) * .75f;		//close to the camera blurs faster



   float sampleNumFloat = RangeMap(blurDepth, 0.0f, 1.0f, 0.0f, 15.0f);   //change based on depth 0-5ish

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

		   float sampleDepth = texture(gTexDepth, newUV).r;   //NEED TO BIND DEPTH BUFFER

			float linearSampleDepth = (2.0f * 0.1f) / (120.0f + 0.1f - sampleDepth * (120.0f - 0.1f));

			float sameDepthNess = 1.0 - AbsValue(linearSampleDepth - lineardepth);
			sameDepthNess -= .2f;	//make the threshold a little smaller for being NOT close to each other
			sameDepthNess = SmoothStep3(sameDepthNess);
          vec4 sampleColor = GetColor(newUV);
		  vec4 addColor = mix(diffuse,sampleColor, sameDepthNess);
          cumulativeColor+=addColor;
      }
   }
   cumulativeColor/=numSamples;

   outColor = cumulativeColor ;
   outColor.a = 1.0;
 }