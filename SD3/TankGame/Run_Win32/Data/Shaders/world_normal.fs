#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal; 

in vec3 passNormal;
in vec3 passTangent;
in vec3 passBiTan;
in vec4 passColor;
in vec2 passUV;
out vec4 outColor;
void main( void )
{


	vec4 diffuse = texture( gTexDiffuse, passUV );
	vec4 normal_color= texture(gTexNormal, passUV);


	// Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   vec3 world_vnormal = normalize(passNormal);

	   // Get the surface to world matrix
   vec3 world_vtan = normalize(passTangent); 
   vec3 world_vbitan = normalize(passBiTan); 
   mat3 surface_to_world = transpose( mat3( world_vtan, world_vbitan, world_vnormal ) ); 

   // range map it to a surface normal
    vec3 surface_normal = normalize( ( normal_color.xyz * vec3( 2.0f, 2.0f, 1.0f )) + vec3( -1.0f, -1.0f, 0.0f ) ); 
    vec3 world_normal = surface_normal * surface_to_world; // tbn
	vec3 normalized = world_normal * .5f;
	vec4 outNormalColor = vec4((normalized.x + .5f), (normalized.y + .5f), (normalized.z + .5f), 1.f);
	//vec4 normalColor = vec4(passNormal.x,1.f,1.f,1.f);
	outColor =  outNormalColor;
}