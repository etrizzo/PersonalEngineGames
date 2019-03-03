#version 420 core
#include "Includes/fog.glsl"
layout(binding = 0) uniform sampler2D gTexDiffuse;


in vec4 passColor;
in vec2 passUV;
in vec3 passNormal;
in vec3 passViewPos;

out vec4 outColor;
void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );

	float indoor = passColor.r;
	float outdoor = passColor.g;

	float actualLightVal = max(indoor, outdoor);

	vec4 lightColor = vec4(actualLightVal, actualLightVal, actualLightVal, 1.0);

	lightColor = vec4(indoor, outdoor, 0.0, 1.0);

	outColor = diffuse * lightColor;
	outColor = ApplyFog( outColor, passViewPos.x);
}