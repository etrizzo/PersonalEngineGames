#version 420 core
#include "Includes/fog.glsl"
#include "Includes/mathutils.glsl"
layout(binding = 0) uniform sampler2D gTexDiffuse;


in vec4 passColor;
in vec2 passUV;
in vec3 passNormal;
in vec3 passViewPos;

uniform float indoorGlowValue = 1.f;

uniform vec3 indoorColorMax = vec3(1.0, .9, .8);
uniform vec3 outdoorColorMax = vec3(.9, .95, 1.0);


in float passOutdoorScalar;

out vec4 outColor;
void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );

	float indoor = passColor.r * indoorGlowValue;
	//need to rangemap indoor value here so that dark areas don't flicker
	indoor = RangeMap(indoor, 0.0, 1.0, .1, 1.0);
	float outdoor = passColor.g * passOutdoorScalar;

	float actualLightVal = max(indoor, outdoor);
	vec3 indoorRGB = indoorColorMax * indoor;
	vec3 outdoorRGB = outdoorColorMax * outdoor;
	vec3 lightRGB = max(indoorRGB, outdoorRGB);

	vec4 lightColor = vec4(lightRGB.r, lightRGB.g , lightRGB.b , 1.0);
	lightColor *= passColor.b;		//blue value is for slightly different tints per side
	lightColor.a = 1.0f;

	//lightColor = vec4(indoor, outdoor, 0.0, 1.0);

	outColor = diffuse * lightColor; 
	outColor = ApplyFog( outColor, passViewPos.x);
}