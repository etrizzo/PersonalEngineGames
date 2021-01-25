#version 420 core
uniform mat4 MODEL;
uniform mat4 PROJECTION;
uniform mat4 VIEW;

in vec3 POSITION;
in vec3 NORMAL;
in vec4 TANGENT;
in vec4 COLOR;
in vec2 UV;

out vec3 passNormal;
out vec3 passTangent;
out vec3 passBiTan;
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
	passNormal = normalize( (vec4( NORMAL, 0.0f ) * MODEL).xyz ); 
	passTangent = normalize( (vec4( TANGENT.xyz, 0.0f ) * MODEL).xyz ); 
	passBiTan = normalize( cross( passTangent, NORMAL ) * TANGENT.w ); 
	gl_Position = clip_pos;
}