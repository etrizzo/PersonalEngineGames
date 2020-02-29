#version 420 core
#include "Includes/lighting.glsl"
#include "Includes/fog.glsl"
#include "Includes/mathutils.glsl"


// Uniforms ==============================================
// Constants
uniform vec3 EYE_POSITION;  // camera related

uniform float DISTANCE = 0.2;
uniform float REFRACT_SPEED = 0.1;
uniform float REFRACT_SCALE = 0.75;
uniform float REFRACT_STRENGTH = 0.01;
uniform vec4 SHALLOW_WATER = vec4(.212, .685, .685, .635);
uniform vec4 DEEP_WATER = vec4(.064, .326, .357, .945);
uniform float FOAM_DISTANCE = .19;
uniform float FOAM_CUTOFF = .9;
uniform float FOAM_NOISE_SCALE = 3.5;
uniform float FOAM_NOISE_SPEED = 0.04;


// lighting - replace with uniform buffer someday
//uniform vec3 LIGHT_POSITION; 
//uniform vec4 LIGHT_COLOR; // xyz color, w intensity

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
layout(binding = 3) uniform sampler2D gTexWaterRamp;
layout(binding = 4) uniform sampler2D gTexRippleRamp;
layout(binding = 5) uniform sampler2D gTexNoise;
layout(binding = 6) uniform sampler2D gTexDepth;
layout(binding = 7) uniform sampler2D gTexColor;		//scene color

layout(binding=8, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 



// Attributes ============================================
in vec2 passUV; 
in vec4 passColor; 
in vec3 passWorldPos;   // new
in vec3 passWorldNormal;// new
in vec3 passWorldTangent;
in vec3 passWorldBitangent;
in vec3 passViewPos;
in vec4 passClipPos;

in vec4 tint;
in vec4 edge_color;
in float depth_factor;

out vec4 outColor; 


vec4 getWorleyColor(float linearDepth, vec3 worldPos, float time){
    vec4 rippleColor = vec4(0.2, 0.55, 0.6, 1.0);
	float rippleStrength = (1.0 - linearDepth) + .2;
	

	float worleyVal = WorleyNoise(worldPos.xz, .5, time);
	
	float worleyPower = worleyVal * worleyVal;// * worleyVal * worleyVal* worleyVal;
	
	float worleyStrength = worleyPower * rippleStrength;
	worleyStrength = clamp(worleyStrength, 0.0, 1.0);
	worleyStrength = SmoothStep3(worleyStrength);
	
	vec4 worleyRamp = texture(gTexRippleRamp, vec2(worleyStrength, 0.5));
	
	vec4 worleyColor = rippleColor * vec4(worleyStrength);
	return worleyColor;
}

// Entry Point ===========================================
void main( void )
{
	//https://stackoverflow.com/questions/26965787/how-to-get-accurate-fragment-screen-position-like-gl-fragcood-in-vertex-shader
	vec3 ndc = passClipPos.xyz /passClipPos.w;
	vec2 viewportCoord = ndc.xy * 0.5 + 0.5;

	vec4 tex_color = texture( gTexDiffuse, passUV ); 

	outColor = tex_color;

	float depth = texture(gTexDepth, viewportCoord).r;   
	//depth *= depth;

	//https://stackoverflow.com/questions/50209415/sampling-the-depth-buffer-and-normalizing-to-0-1-directx
	float lineardepth = (2.0f * 0.1f) / (120.0f + 0.1f - depth * (120.0f - 0.1f));		//scene depth

	float objectDepth = RangeMap(passClipPos.w, 0.1f, 120.f, 0.f, 1.f);
	//float objectDepth = clamp(camToWaterLength / 120.0f, 0.0f, 1.0f);
	float waterDepth = lineardepth - objectDepth;
	float divideByDistance = waterDepth / DISTANCE;
	float saturated = clamp(divideByDistance, 0.0, 1.0);
	//saturated = (saturated + 1) * .5f;		//smooth falloff
	//STEP 2 COMPLETE - DEPTH FADE 
	//outColor = vec4(vec3(saturated), 1.0);


	vec4 waterColor = mix(SHALLOW_WATER, DEEP_WATER, saturated);
	vec2 offset = (passUV * REFRACT_SCALE) + vec2(GAME_TIME * REFRACT_SPEED);
	float noise = texture(gTexNoise, offset).r;
	noise = noise * REFRACT_STRENGTH;
	vec2 refractedCoord = viewportCoord + vec2(noise);
	vec4 sceneColor = texture(gTexColor, refractedCoord);
	vec4 lerpedColor = mix(sceneColor, waterColor, waterColor.a);
	//STEP 5 COMPLETE - REFRACTION
	outColor = vec4(lerpedColor.xyz, 1.0);
	
	//WORLEY
	float dropoff = SmoothStop3(1.0 - SmoothStep3(objectDepth));
	vec3 worldPos = passWorldPos * vec3(1.0, 1.0, .65);
	float worleyFoamVal = WorleyNoise(worldPos.xz, .5, GAME_TIME);
	float worleyFoamPower = worleyFoamVal * worleyFoamVal;// * worleyVal * worleyVal* worleyVal;
	vec4 worleyColor = getWorleyColor(dropoff * .8, worldPos, GAME_TIME);

	//FOAM:
	float foamDivide = waterDepth / FOAM_DISTANCE;
	float foamSaturated = clamp(foamDivide, 0.0, 1.0);
	foamSaturated = SmoothStep3(foamSaturated * foamSaturated * foamSaturated);
	float foamCutoffAmount = foamSaturated * FOAM_CUTOFF;
	//get foam noise
	vec2 foamOffset = (passUV * FOAM_NOISE_SCALE) + vec2(GAME_TIME * FOAM_NOISE_SPEED);
	float foamNoise = texture(gTexNoise, foamOffset).b;
	foamNoise = SmoothStep3(foamNoise);
	//foamNoise = worleyFoamPower * .2 * foamCutoffAmount;
	float foamValue =step(foamCutoffAmount, mix(foamNoise, worleyFoamPower, .4));
	//foamValue += step(foamCutoffAmount, worleyFoamPower * .8);// * worleyFoamPower * .8;
	float worleyFoam = step(foamCutoffAmount - (worleyFoamPower), foamNoise);
	//foamValue = step(foamCutoffAmount, .5);		//foam line (no noise)
	
	outColor += vec4(foamValue * dropoff);
	//outColor += vec4(worleyFoam * dropoff * (worleyFoamPower * worleyFoamPower * worleyFoamPower) * .3f);

	outColor = outColor + worleyColor;
	outColor.a = 1.0;

	//outColor = vec4(vec3(foamValue), 1.0);

	//END WORLEY


   // OLD COLOR

	//float wRangeMapped = RangeMap(passClipPos.w, 0.1f, 120.f, 0.f, 1.f);
	//float foamLine = 1 - clamp(depth_factor * (lineardepth - wRangeMapped), 0.0f, 0.8f);
	//
	//vec4 foamRamp = texture(gTexWaterRamp, vec2(foamLine, 0.5));
	//
	//float depthFoam = foamLine * (lineardepth + .5);
	//float extremefied = SmoothStep3(depthFoam);
	//vec4 newTint = vec4(.3, .5, .5, .9);
	//outColor = mix(newTint, edge_color, extremefied);
	//outColor = outColor * foamRamp.r * clamp(foamLine, 0.75, 1.0);
	//outColor.a = 0.9;

	// END OLD COLOR




   
}