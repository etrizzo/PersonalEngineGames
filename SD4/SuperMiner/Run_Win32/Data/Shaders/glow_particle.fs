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

	float particleAlpha = (passColor.a * indoorGlowValue);
	float invAlpha = 1.f - particleAlpha;
	if (diffuse.a < invAlpha)
	{
		discard;
	}

	//if it's bright out, don't
	if (passOutdoorScalar > .5f)
	{
		discard;
	}

	float invBrightness = 1.f - (passOutdoorScalar * 2.f);
	outColor = passColor; // * particleAlpha;
	outColor.a = .7 * invBrightness;
}