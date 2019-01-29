#version 420 core
#include "Includes/lighting.glsl"
#include "Includes/fog.glsl"



// Uniforms ==============================================
// Constants
uniform vec3 EYE_POSITION;  // camera related


// lighting - replace with uniform buffer someday
//uniform vec3 LIGHT_POSITION; 
//uniform vec4 LIGHT_COLOR; // xyz color, w intensity

// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;

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

out vec4 outColor; 

// Entry Point ===========================================
void main( void )
{
    // Get the surface colour
   vec2 uv_offset = passUV + vec2(GAME_TIME * .1f); 
   vec4 tex_color = texture( gTexDiffuse, passUV + uv_offset ); 
   vec3 normal_color = texture( gTexNormal, passUV + uv_offset ).xyz;
  // vec3 emissive_color = texture( gTexEmissive, passUV ).xyz; 

   // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   vec3 world_vnormal = normalize(passWorldNormal);

   // Get the surface to world matrix
   vec3 world_vtan = normalize(passWorldTangent); 
   vec3 world_vbitan = normalize(passWorldBitangent); 
   mat3 surface_to_world = transpose( mat3( world_vtan, world_vbitan, world_vnormal ) ); 

   // range map it to a surface normal
   vec3 surface_normal = normalize( normal_color * vec3( 2.0f, 2.0f, 1.0f ) + vec3( -1.0f, -1.0f, 0.0f ) ); 
   vec3 world_normal = surface_normal * surface_to_world; // tbn

   vec3 eye_dir = normalize( EYE_POSITION - passWorldPos ); 

   //calculate lighting in Includes/lighting.glsl
   light_factor_t lf = CalculateLighting( passWorldPos, 
      eye_dir, 
      world_normal, 
      SPECULAR_AMOUNT, 
      SPECULAR_POWER ); 


   // Add color of the lit surface to the reflected light
   // to get the final color; 
   vec4 final_color = (vec4(lf.diffuse, 1) * tex_color * passColor) + vec4(lf.specular, 0); 
  // final_color.xyz = ADD(final_color.xyz, emissive_color); 

   final_color = clamp(final_color, vec4(0), vec4(1) ); // not necessary - but overflow should go to bloom target (bloom effect)
   final_color = ApplyFog(final_color, passViewPos.z);
   outColor = final_color;
}