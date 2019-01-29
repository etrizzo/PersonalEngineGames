#version 420 core
// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 


in vec3 POSITION;
in vec3 NORMAL;
in vec4 TANGENT;
in vec4 COLOR;
in vec2 UV;

out vec3 passNormal;
out vec3 passTangent;
out vec4 passColor;
out vec2 passUV;

void main( void )
{
 	vec4 local_pos = vec4( POSITION, 1.0f );  

   	vec4 world_pos = MODEL *local_pos ; 
  	vec4 camera_pos = VIEW * world_pos ; 
   	vec4 clip_pos = PROJECTION * camera_pos ; 
   	
	passColor = COLOR;
	passUV = UV;
	passTangent = TANGENT.xyz;
	gl_Position = clip_pos;
}