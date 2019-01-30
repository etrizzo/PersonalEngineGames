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

vec4 GetColor(vec2 uvs)
{
   return texture(gTexDiffuse, uvs);
}

vec4 GetRandom(vec2 uvs)
{
   //return texture(gTexDisplacement, uvs);
   return texture(gTexDisplacement, uvs + (  TEXEL_SIZE_X * 15.0 * sin(GAME_TIME)));
}

//works
vec2 GetGradient(vec2 uvs, float delta)
{
   vec2 displacementX=vec2(delta * TEXEL_SIZE_X,0);
   vec2 displacementY= vec2(0, delta * TEXEL_SIZE_Y);
   float magicNumber = .333;        //this doesn't seem to change anything
    //dotting a change in color (from +d to -d) with a dark gray??? and dividing by the change?
    //if the change is small across the area, this will return a number closer to 1 / delta.
    // if not it will become  <1 / delta which is ... smaller
   float x = dot((GetColor(uvs + displacementX)-GetColor(uvs - displacementX)).xyz, vec3(magicNumber));
   float y = dot((GetColor(uvs + displacementY)-GetColor(uvs - displacementY)).xyz, vec3(magicNumber));
   
   return vec2(x,y);
}


vec4 GetEdgeColor(vec2 passUV)
{
   vec4 edgeColor = vec4(1.0);
   int halfSize = 1;
   vec4 diffuseColor = texture(gTexDiffuse, passUV);
   for(int x = (-1 * halfSize); x < halfSize; x++ )
   {
      for(int y = (-1 * halfSize); y < halfSize; y++ )
      {
         vec2 newUVs = passUV;
         newUVs.x += (x * TEXEL_SIZE_X);
         newUVs.y += (y * TEXEL_SIZE_Y);
         vec4 sampleColor = GetColor(newUVs);
         if (DoColorsExceedEdgeThreshold(diffuseColor, sampleColor, 0.2)){
            edgeColor = vec4(0.5,0.5,0.5,1.0);
         }
      }
   }
   return edgeColor;
}


void main() 
{
	float scaledTime = GAME_TIME * .05;
	vec2 dispUVs = vec2(passUV.x + scaledTime, passUV.y + scaledTime);
 	vec4 displacement = texture(gTexDisplacement, dispUVs);


   vec4 edgeColor = GetEdgeColor(passUV);
   

   // sample (gather) our texel colour for this UV
   vec4 diffuse = GetColor(passUV);

   

   vec2 positiveEdgePosition = passUV;
   vec2 negativeEdgePosition = passUV;

   vec3 watercolor3 = vec3(0.0);

   int sampleNum = 12;
   float count2 = 0.0;

   vec2 texelSize = vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);

   for (int i = 0; i < sampleNum; i++)
   {
      // gradient for wash effect (white on green screen)
        
        //started at .0001
        //larger scales make it more grainy like the glass effect in photoshop bc you're sampling from further away?
        float randomOffsetScale = .0001;
        
        //get some random offsets?
        //get a random vec2, range map to [-.5 , .5]
        //then scale it down to be .0001x size        vv rangemapping to [-.5,5]
        vec2 randomSmallOffset3 = randomOffsetScale * ((GetRandom(positiveEdgePosition).xy - vec2(.5)) * texelSize);
        vec2 randomSmallOffset4 = randomOffsetScale * ((GetRandom(negativeEdgePosition).xy - vec2(.5)) * texelSize);
        
        float delta = .01;    //starts at 2.0
        //vec2 getGrad is a higher value if change in color is closer to .3 (highest is (1,1)/delta == (.5,.5)
        //gr3 and gr4 are pretty much edge gradients with a really small random offset
        //the delta makes it fuzzier
       vec2 gr3=GetGradient(positiveEdgePosition,delta) + randomSmallOffset3;         //gr3 and pos3 seem to add a little more fine detail
       vec2 gr4=GetGradient(negativeEdgePosition,delta) + randomSmallOffset4;         //gr4 and pos4 get it very blobby
       
  
        float fact = 1.0 - (float(i) /(float(sampleNum)* 1.0));        //closer samples are more effective?
        
        
        float gradiantScale3 = .5;    //started at .25
        float gradiantScale4 = .75;    //started at .5
        
        //scales down the range of where we sample our noise
        //so that we get a chunkier texture?
        //bc we're sampling from a lower resolution area?
        float noiseSampleScale = .1;     //started at .07
        
        
        // colors + wash effect on gradients:
        // color gets lost from dark areas
        //to bright areas

        //without the normalize, the image gets much crisper
        //bc the values are much smaller? so normalizing makes the displacement larger
        //moving the positions creates like a smudging effect because the color accumulates along the gradiant
        vec2 positiveUVChange = (gradiantScale3*normalize(gr3)) + .5*(GetRandom(passUV*noiseSampleScale).xy-.5);
        vec2 negativeUVChange = (gradiantScale4*normalize(gr4)) + .5*(GetRandom(passUV*noiseSampleScale).xy-.5);
        positiveEdgePosition+= positiveUVChange * vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);     //dark areas - positive along gradiant
        negativeEdgePosition-= positiveUVChange * vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);     //bright areas - negative along gradiant
        
        float f1=3.0 *fact;     //amplify close to me
        float f2=4.0 *(.7-fact);    //amplify things within .7 of me??
        //how close you are * (25% your color + 40%noiseColor));
        watercolor3+=f1*(GetColor(positiveEdgePosition).xyz +vec3(.25)+ .1*GetRandom(positiveEdgePosition*1.0).xyz);
        watercolor3+=f2*(GetColor(negativeEdgePosition).xyz +vec3(.25)+ .1*GetRandom(negativeEdgePosition*1.0).xyz);
        
        count2+=f1+f2;      //base color factor?
        //count+=fact;        //edge factor?
   }

   watercolor3/= (count2 * 1.65);  //weight of the blobby color

   vec4 watercolor = vec4(watercolor3, 1.0);
  
   outColor = (watercolor * passColor * edgeColor) ;
   //vec4 gradiantColor = vec4(normalize(GetGradient(passUV, 1.0)), 0.0, 1.0);
   //vec4 gradiantColor = vec4(GetGradient(passUV, 1.0), 0.0, 1.0);
   //outColor = gradiantColor;
}