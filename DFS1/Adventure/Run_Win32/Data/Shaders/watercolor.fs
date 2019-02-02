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

vec2 GetPerpendicular(vec2 a)
{
  return a.yx * vec2(1,-1);
}

vec4 GetColor(vec2 uvs)
{
   return texture(gTexDiffuse, uvs);
}

vec4 GetRandomWithTimeDisplacement(vec2 uvs)
{
   //return texture(gTexDisplacement, uvs);
  return texture(gTexDisplacement, uvs + vec2(-TEXEL_SIZE_X, TEXEL_SIZE_Y * .5) * 70.0 );
   return texture(gTexDisplacement, uvs + (  TEXEL_SIZE_X * 150.0 * sin(GAME_TIME)));
}

vec4 GetRandom(vec2 uvs)
{
   //return texture(gTexDisplacement, uvs);
   return texture(gTexDisplacement, uvs);
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

//gives a down-resed, squared noise value
float htPattern(vec2 pos)
{
    float p;
    vec2 timeDisplacement = vec2(-TEXEL_SIZE_X, TEXEL_SIZE_Y * .5) * 100.0 * GAME_TIME * .15;
    float r=GetRandom(pos*.1 + vec2(.0255, .0265) + timeDisplacement).x;     //down-res the noise, [0,1] value
    float rAdjusted = (r +.3) * (GetRandom(pos + timeDisplacement ).r + .4) ;         //combine the low resolution black/white noise with regular noise to make it crunchier
    p=clamp(rAdjusted ,0.0,1.0); //square value of noise to get starker difference, then subtract to get more darks
    return p * .9;
}

//gets the grayscale value at pos 
float GetGrayscale(vec2 pos)
{
    return length(GetColor(pos).xyz);//+0.0001*length(pos-0.5);
    //return dot(getCol(pos).xyz,vec3(.333));
}
    
//
vec4 getBWDist(vec2 pos)
{
    //smooth step from .9 to 1.1 wherethe interpolation is a weight between the gray value and a down-res of the noise at that point.
    //so light/dark areas will be less noisy while neutral areas will have a noise pattern.
    //the weight of the grayscale is the actual edges, the weight of htPattern is the full screen noisyness
    return vec4(smoothstep(.9,1.1,GetGrayscale(pos)*.95+htPattern(pos*.4)*.55));
}


void main() 
{
	float scaledTime = GAME_TIME * .05;
	vec2 dispUVs = vec2(passUV.x + scaledTime, passUV.y + scaledTime);
 	vec4 displacement = texture(gTexDisplacement, dispUVs);


   //vec4 edgeColor = GetEdgeColor(passUV);
   

   // sample (gather) our texel colour for this UV
   vec4 diffuse = GetColor(passUV);

   
   vec2 positiveEdgePosition = passUV;
   vec2 negativeEdgePosition = passUV;

   vec2 positiveWatercolorPosition = passUV;
   vec2 negativeWatercolorPosition = passUV;

   vec3 edgeColor3 = vec3(0.0);
   vec3 watercolor3 = vec3(0.0);

   int sampleNum = 12;
   float count = 0.0;
   float count2 = 0.0;

   vec2 texelSize = vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);

   float vign = 0.5 + 0.5*16.0*passUV.x*passUV.y*(1.0-passUV.x)*(1.0-passUV.y);

   for (int i = 0; i < sampleNum; i++)
   {
      // gradient for wash effect (white on green screen)
        
        //started at .0001
        //larger scales make it more grainy like the glass effect in photoshop bc you're sampling from further away?
        float randomOffsetScale = .00005;
        
        //get some random offsets?
        //get a random vec2, range map to [-.5 , .5]
        //then scale it down to be .0001x size        vv rangemapping to [-.5,5]
        vec2 randomSmallOffset1 = randomOffsetScale * ((GetRandomWithTimeDisplacement(positiveEdgePosition).xy - vec2(.5)) * texelSize);
        vec2 randomSmallOffset2 = randomOffsetScale * ((GetRandomWithTimeDisplacement(negativeEdgePosition).xy - vec2(.5)) * texelSize);
        
        vec2 randomSmallOffset3 = randomOffsetScale * ((GetRandomWithTimeDisplacement(positiveWatercolorPosition).xy - vec2(.5)) * texelSize);
        vec2 randomSmallOffset4 = randomOffsetScale * ((GetRandomWithTimeDisplacement(negativeWatercolorPosition).xy - vec2(.5)) * texelSize);
        
        //==============================
        //calculate for watercolor effect
        float delta = 1.1;    //starts at 2.0
        //vec2 getGrad is a higher value if change in color is closer to .3 (highest is (1,1)/delta == (.5,.5)
        //gr3 and gr4 are pretty much edge gradients with a really small random offset
        //the delta makes it fuzzier
        vec2 gr3=GetGradient(positiveWatercolorPosition,delta) + randomSmallOffset3;         //gr3 and pos3 seem to add a little more fine detail
        vec2 gr4=GetGradient(negativeWatercolorPosition,delta) + randomSmallOffset4;         //gr4 and pos4 get it very blobby
      
        float distanceFactor = 1.0 - (float(i) /(float(sampleNum)* 1.0));        //closer samples are more effective?
        
        
        float gradiantScale3 = 1.5 * ((sin(GAME_TIME + 2) + 2.0f) * .25);    //started at .25
        float gradiantScale4 = .75;    //started at .5
        
        //scales down the range of where we sample our noise
        //so that we get a chunkier texture?
        //bc we're sampling from a lower resolution area?
        float noiseSampleScale = .05;     //started at .07
        
        // colors + wash effect on gradients:
        // color gets lost from dark areas to bright areas
        //without the normalize, the image gets much crisper
        //bc the values are much smaller? so normalizing makes the displacement larger
        //moving the positions creates like a smudging effect because the color accumulates along the gradiant
        vec2 positiveUVChange = (gradiantScale3*normalize(gr3)) + .5*(GetRandomWithTimeDisplacement(passUV*noiseSampleScale).xy-.7);
        vec2 negativeUVChange = (gradiantScale4*normalize(gr4)) + .5*(GetRandomWithTimeDisplacement(passUV*noiseSampleScale).xy-.7);
        positiveWatercolorPosition+= positiveUVChange * vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);     //dark areas - positive along gradiant
        negativeWatercolorPosition-= positiveUVChange * vec2(TEXEL_SIZE_X, TEXEL_SIZE_Y);     //bright areas - negative along gradiant
        
        float f1=6.0 * distanceFactor;     //amplify close to me
        float f2=4.0 *(.7-distanceFactor);    //amplify things within .7 of me??
        //how close you are * (25% your color + 40%noiseColor));
        watercolor3+=f1*(GetColor(positiveWatercolorPosition).xyz +vec3(.25)+ .05*(GetRandomWithTimeDisplacement(positiveEdgePosition*1.0).xyz - .5));
        watercolor3+=f2*(GetColor(negativeWatercolorPosition).xyz +vec3(.25)+ .05*(GetRandomWithTimeDisplacement(negativeWatercolorPosition*1.0).xyz - .5));
        
        count2+=f1+f2;      //base color factor?
        //count+=fact;        //edge factor?


        //==============================
        //calculate for edge effect

        
        float edgeDelta = vign * 1.5;
        vec2 gr =GetGradient(positiveEdgePosition, edgeDelta)+ randomSmallOffset1;
        vec2 gr2=GetGradient(negativeEdgePosition, edgeDelta)+ randomSmallOffset2;
        
        //amplify the length so that the strong ones are 1 and everything else is at least visible
        float lengthScale = 10.0;     //started at 10.0, lower val means lighter outlines
        float gr1Strength=clamp(lengthScale*length(gr),0.,1.);
        float gr2Strength=clamp(lengthScale*length(gr2),0.,1.);

        // outlines:
        // stroke perpendicular to gradient
        positiveEdgePosition +=.8 *normalize(GetPerpendicular(gr)) * texelSize;
        negativeEdgePosition -=.8 *normalize(GetPerpendicular(gr2))* texelSize;

        edgeColor3+=distanceFactor*mix(vec3(1.2),getBWDist(positiveEdgePosition).xyz*2.,gr1Strength); //use the strength of the gradient to determine
        edgeColor3+=distanceFactor*mix(vec3(1.2),getBWDist(negativeEdgePosition).xyz*2.,gr2Strength);    //if this is an edge or nah ( too low will be mostly wite)
        
        count+=distanceFactor;      //edge factor?
        
   }
   edgeColor3/= count  * 2.5;
   watercolor3/= (count2 * 1.45);  //weight of the blobby color

   vec4 waterColor = vec4(watercolor3, 1.0);
   waterColor = mix(waterColor, diffuse,.15);
   vec4 edgeColor = vec4(edgeColor3, 1.0);
  

   //float r = length((passUV-texelSize*.5)/TEXEL_SIZE_X);
   //float vign = (1.0-r) * 2.0;
   //float vign = 0.5 + 0.5*16.0*passUV.x*passUV.y*(1.0-passUV.x)*(1.0-passUV.y);
   vec4 finalColor = clamp(clamp(edgeColor*.85+.15,0.,1.)*waterColor,0.,1.);
   outColor = (finalColor * passColor) ;
   //outColor = mix(outColor, diffuse, .1)
   //outColor = getBWDist(passUV);
   //outColor = vec4(vign * vign);
   //outColor = waterColor;
   outColor.a = 1.0;
   //vec4 gradiantColor = vec4(normalize(GetGradient(passUV, 1.0)), 0.0, 1.0);
   //vec2 randomSmallOffset = .0001 * ((GetRandomWithTimeDisplacement(positiveEdgePosition).xy - vec2(.5)) * texelSize);
   //vec2 gr =GetGradient(positiveEdgePosition, .5)+ randomSmallOffset;
    

   //vec4 gradiantColor = vec4(gr.x, gr.y, 0.0, 1.0);
   //outColor = gradiantColor;
}