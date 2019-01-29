#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
out vec4 outColor;
void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );
	vec4 temp = diffuse * passColor;
	outColor = vec4(1.f - temp.r, 1.f - temp.g, 1.f - temp.b, ( temp.a *.6f));
}