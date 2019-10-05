#version 420 core
#include "Includes/lighting.glsl"
#include "Includes/fog.glsl"
#include "Includes/mathutils.glsl"


// Uniforms ==============================================
// Constants
uniform vec3 EYE_POSITION;  // camera related



// lighting - replace with uniform buffer someday
//uniform vec3 LIGHT_POSITION; 
//uniform vec4 LIGHT_COLOR; // xyz color, w intensity

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;
layout(binding = 3) uniform sampler2D gTexWaterRamp;
layout(binding = 4) uniform sampler2D gTexRippleRamp;

layout(binding = 6) uniform sampler2D gTexDepth;

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


// Entry Point ===========================================
void main( void )
{
	//https://stackoverflow.com/questions/26965787/how-to-get-accurate-fragment-screen-position-like-gl-fragcood-in-vertex-shader
	vec3 ndc = passClipPos.xyz /passClipPos.w;
	vec2 viewportCoord = ndc.xy * 0.5 + 0.5;

	vec4 tex_color = texture( gTexDiffuse, passUV ); 

	outColor = tex_color;

   float depth = texture(gTexDepth, viewportCoord).r;   //NEED TO BIND DEPTH BUFFER
   //depth *= depth;

   //https://stackoverflow.com/questions/50209415/sampling-the-depth-buffer-and-normalizing-to-0-1-directx
   float lineardepth = (2.0f * 0.1f) / (120.0f + 0.1f - depth * (120.0f - 0.1f));

  

   
   float wRangeMapped = RangeMap(passClipPos.w, 0.1f, 120.f, 0.f, 1.f);
    float foamLine = 1 - clamp(depth_factor * (lineardepth - wRangeMapped), 0.0f, 0.8f);

	vec4 foamRamp = texture(gTexWaterRamp, vec2(foamLine, 0.5));

	float depthFoam = foamLine * (lineardepth + .5);
	float extremefied = SmoothStep3(depthFoam);

	vec4 newTint = vec4(.3, .5, .5, .9);

	//outColor = vec4(vec3(extremefied), 1.0);
	//outColor = newTint;
	//return;


   //outColor = vec4(vec3(wRangeMapped), 1.0);
   //outColor =  vec4(.5, .75, 1.0, 1.0) * foamLine *  vec4(.8, 1.0, 1.0, 1.0);
   outColor = mix(newTint, edge_color, extremefied);
   //outColor = tint * foamRamp * clamp(foamLine, 0.45, 1.0);
   outColor = outColor * foamRamp.r * clamp(foamLine, 0.75, 1.0);
   outColor.a = 0.9;
   //outColor = vec4(vec3(foamLine), 1.0);

   //WORLEY
    vec4 rippleColor = vec4(0.4, 0.65, 0.7, 1.0);
	float rippleStrength = (1.0 - lineardepth) + .2;
	

   float worleyVal = WorleyNoise(passWorldPos.xz, 1.5, GAME_TIME);

   float worleyPower = worleyVal * worleyVal * worleyVal * worleyVal* worleyVal;

   float worleyStrength = worleyPower * rippleStrength;
   worleyStrength = clamp(worleyStrength, 0.0, 1.0);

   vec4 worleyRamp = texture(gTexRippleRamp, vec2(worleyStrength, 0.5));

   //float worleyValueFinal = worleyRamp.r * worleyRamp.r;

   vec4 addColor = rippleColor * vec4(worleyStrength);
   outColor = outColor + addColor;

   //outColor = vec4(vec3(lineardepth), 1.0);

  //  // Get the surface colour
  // vec2 uv_offset = passUV + vec2(GAME_TIME * .1f); 
  // vec4 tex_color = texture( gTexDiffuse, passUV + uv_offset ); 
  // vec3 normal_color = texture( gTexNormal, passUV + uv_offset ).xyz;
  //// vec3 emissive_color = texture( gTexEmissive, passUV ).xyz; 
  //
  // // Interpolation is linear, so normals become not normal
  // // over a surface, so renormalize it. 
  // vec3 world_vnormal = normalize(passWorldNormal);
  //
  // // Get the surface to world matrix
  // vec3 world_vtan = normalize(passWorldTangent); 
  // vec3 world_vbitan = normalize(passWorldBitangent); 
  // mat3 surface_to_world = transpose( mat3( world_vtan, world_vbitan, world_vnormal ) ); 
  //
  // // range map it to a surface normal
  // vec3 surface_normal = normalize( normal_color * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) ); 
  // vec3 world_normal = surface_normal * surface_to_world; // tbn
  //
  // vec3 eye_dir = normalize( EYE_POSITION - passWorldPos ); 
  //
  // //calculate lighting in Includes/lighting.glsl
  // light_factor_t lf = CalculateLighting( passWorldPos, 
  //    eye_dir, 
  //    world_normal, 
  //    SPECULAR_AMOUNT, 
  //    SPECULAR_POWER ); 
  //
  //
  // // Add color of the lit surface to the reflected light
  // // to get the final color; 
  // vec4 final_color = (vec4(lf.diffuse, 1) * tex_color * passColor) + vec4(lf.specular, 0); 
  //// final_color.xyz = ADD(final_color.xyz, emissive_color); 
  //
  // final_color = clamp(final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
  // final_color = ApplyFog(final_color, passViewPos.z);
  // outColor = final_color;
}