#version 420 core

// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 

uniform vec4 TINT = vec4(.5, .75, 1.0, 8.0);
uniform vec4 EDGE_COLOR = vec4(.8, 1.0, 1.0, 1.0);
uniform float DEPTH_FACTOR = 5.0;

uniform float WAVE_SPEED = 6;
uniform float WAVE_AMP = .08f;

layout(binding = 5) uniform sampler2D gTexNoise;

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
out vec3 passWorldPos;     // new
out vec3 passWorldNormal;  // new
out vec3 passWorldTangent;
out vec3 passWorldBitangent;
out vec3 passViewPos;
out vec4 passClipPos;

out vec4 tint;
out vec4 edge_color;
out float depth_factor;


// Entry Point ===========================================
void main( void )
{

   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos ; 


   float noiseSample = texture(gTexNoise, UV).x;
   world_pos.y += sin(GAME_TIME * WAVE_SPEED * noiseSample) * WAVE_AMP;
   world_pos.x += cos(GAME_TIME * WAVE_SPEED * noiseSample) * WAVE_AMP;

   vec4 camera_pos = VIEW * world_pos ; 
   vec4 clip_pos = PROJECTION * camera_pos ; 
 
   passUV = UV; 
   passColor = COLOR;

   

   // new
   passWorldPos = world_pos.xyz;  
   passWorldNormal = (vec4( NORMAL, 0.0f ) * MODEL).xyz; 
   passWorldTangent = TANGENT.xyz;
   passWorldBitangent = normalize( cross( passWorldTangent, passWorldNormal ) * TANGENT.w ); 
   passViewPos = camera_pos.xyz;
   passClipPos = clip_pos;

   tint = TINT;
   edge_color = EDGE_COLOR;
   depth_factor = DEPTH_FACTOR;

   gl_Position = clip_pos; // we pass out a clip coordinate
   
}
