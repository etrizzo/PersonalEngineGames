#version 420 core
uniform sampler2D gTexDiffuse;
in vec3 passTangent;
in vec3 passBiTan;
in vec4 passColor;
in vec2 passUV;
out vec4 outColor;
void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );
	vec3 normalized = passBiTan * .5f;
	vec4 normalColor = vec4((normalized.x + .5f), (normalized.y + .5f), (normalized.z + .5f), 1.f);
	//vec4 normalColor = vec4(passNormal.x,1.f,1.f,1.f);
	outColor =  normalColor;
}