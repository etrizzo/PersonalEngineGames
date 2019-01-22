#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
layout(location = 0) in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
out vec4 passColor;
out vec2 passUV;
out vec2 passBorderUV;

vec2 BORDER_UVS[] = { vec2(0.0), vec2(0.0f, 1.0f), vec2(1.0f, 0.0f), vec2(1.0f) };

void main( void )
{
	vec4 local_pos = vec4( POSITION, 1 );
	vec4 clip_pos =  PROJECTION * VIEW * local_pos;
	passColor = COLOR;
	passUV = UV;
	passBorderUV = BORDER_UVS[gl_VertexID % 4]; 
	gl_Position = clip_pos;
}