#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
in vec2 passBorderUV;
out vec4 outColor;
void main( void )
{
	//outColor = vec4(1,1,0,1);
	float min = .05;
	float max = .95;
	vec4 diffuse = texture( gTexDiffuse, passUV );
	outColor = diffuse * passColor;
	if (passBorderUV.x < min || passBorderUV.x > max || passBorderUV.y < min || passBorderUV.y > max){
		float scale = .4;
		outColor = outColor * vec4(scale,scale,scale,1);
	} 
}