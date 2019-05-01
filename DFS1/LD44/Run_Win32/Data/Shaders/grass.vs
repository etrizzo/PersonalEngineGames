#version 420 core
#include "Includes/mathutils.glsl"

layout(binding = 4) uniform sampler2D gTexGradient;


// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 

uniform vec4 TINT = vec4(1);


layout(binding=8, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 


// Attributes ============================================
// Inputs
in vec3 POSITION;
in vec3 NORMAL;   // new
in vec4 COLOR;
in vec4 TANGENT;
in vec2 UV; 

// Outputs
out vec2 passUV; 
out vec4 passColor; 
//out vec3 normalizedWorldPos;
out vec2 worldUV;

// Entry Point ===========================================
void main( void )
{
   /*
   vec4 local_pos = vec4( POSITION, 1 );
   vec4 clip_pos =  PROJECTION * VIEW * local_pos;
   passColor = COLOR;
   passUV = UV;
   gl_Position = clip_pos;
   */
  
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL *local_pos ; 


   float radius = 25.0;       //lol this should be a param xdd
   vec3 normWorld = world_pos.xyz;
   normWorld.x = RangeMap(normWorld.x, -radius, radius, 0.0, 1.0);
   normWorld.z = RangeMap(normWorld.z, -radius, radius, 0.0, 1.0);

   worldUV = vec2(normWorld.x, normWorld.y);
   worldUV = worldUV + vec2(GAME_TIME * .3f, 0.0f); 
 
   passUV = UV; 
   passColor = COLOR * TINT; 


   float vertexHeight = UV.y;

   //float rangeMappedHeight = RangeMap(vertexHeight, 35.0f, 45.0f, 0.0f, 1.0f);

   float rangeMappedHeight = clamp(vertexHeight, 0.0f, 1.0f);
   //rangeMappedHeight-=.5f;

   float heightFactor = rangeMappedHeight * 1.f;

   vec4 gradientColor = texture( gTexGradient, worldUV );

   vec4 movement = vec4((gradientColor.x - .25f) * .25f * heightFactor, 0.0f, 0.0f, 0.0f);

   world_pos += movement;

   vec4 camera_pos = VIEW * world_pos ; 
   vec4 clip_pos = PROJECTION * camera_pos ; 

   gl_Position = clip_pos; // we pass out a clip coordinate
   
}
