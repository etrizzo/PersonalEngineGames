#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
layout(location = 0) in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 passColor;
out vec2 passUV;
void main( void )
{
	vec4 local_pos = vec4( POSITION, 1 );
	vec4 clip_pos =  PROJECTION * VIEW * local_pos;
	passColor = COLOR;
	passUV = UV;
	gl_Position = clip_pos;
}