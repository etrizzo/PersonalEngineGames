#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor;
in vec2 passUV;
out vec4 outColor;

void main( void )
{
	outColor = vec4( 0, 0, 1, 1);
}