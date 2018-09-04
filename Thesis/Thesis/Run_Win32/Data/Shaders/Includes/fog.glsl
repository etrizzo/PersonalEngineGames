//--------------------------------------------------------------------------------------
// DEFINES
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

layout(binding=0, std140) uniform uFogBlock 
{
   // Fog
   vec4 FOG_COLOR; 
   float FOG_NEAR_PLANE;
   float FOG_NEAR_FACTOR;
   float FOG_FAR_PLANE;
   float FOG_FAR_FACTOR;
};


//--------------------------------------------------------------------------------------
// FUNCTIONS
//--------------------------------------------------------------------------------------

vec4 ApplyFog( vec4 color, float view_depth )
{
   float fog_factor = smoothstep( FOG_NEAR_PLANE, FOG_FAR_PLANE, view_depth ); 
   fog_factor = FOG_NEAR_FACTOR + (FOG_FAR_FACTOR - FOG_NEAR_FACTOR) * fog_factor;
   color = mix( color, FOG_COLOR, fog_factor ); 

   return color; 
}