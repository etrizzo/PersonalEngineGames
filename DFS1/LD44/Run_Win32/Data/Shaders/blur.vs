#version 420 core
// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 

layout(location = 0) in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 passColor;
out vec2 passUV;
out vec3 passViewPos;
out mat4 passView;


void main( void )
{
 	vec4 local_pos = vec4( POSITION, 1.0f );  

   	vec4 world_pos = MODEL *local_pos ; 
  	vec4 camera_pos = VIEW * world_pos ; 
   	vec4 clip_pos = PROJECTION * camera_pos ; 
   	
	passColor = COLOR;
	passUV = UV;
	passViewPos = camera_pos.xyz;
	gl_Position = clip_pos;
}