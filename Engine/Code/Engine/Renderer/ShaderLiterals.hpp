#pragma once

const char* DEFAULT_VS = R"(
#version 420 core

// Uniforms ==============================================
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION; 

// Attributes ============================================
// Inputs
in vec3 POSITION;
in vec3 NORMAL;   // new
in vec4 COLOR;
in vec2 UV; 

// Outputs
out vec2 passUV; 
out vec4 passColor; 
out vec3 passWorldPos;     // new
out vec3 passWorldNormal;  // new

// Entry Point ===========================================
void main( void )
{
   /*
   vec4 local_pos = vec4( POSITION, 1 );
   vec4 clip_pos =  PROJECTION * VIEW * local_pos;
   passColor = COLOR;
   passUV = UV;
   gl_Position = clip_pos;
   */
  
   vec4 local_pos = vec4( POSITION, 1.0f );  

   vec4 world_pos = MODEL * local_pos ; 
   vec4 camera_pos = VIEW * world_pos ; 
   vec4 clip_pos = PROJECTION * camera_pos ; 
 
   passUV = UV; 
   passColor = COLOR; 

   // new
   passWorldPos = world_pos.xyz;  
   passWorldNormal = (vec4( NORMAL, 0.0f ) * MODEL).xyz; 

   gl_Position = clip_pos; // we pass out a clip coordinate
   
}
)";

const char* DEFAULT_FS = R"(
#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;


in vec4 passColor;
in vec2 passUV;
in vec3 passNormal;
out vec4 outColor;
void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );
	outColor = diffuse * passColor;
}
)";


const char* INVALID_VS = R"(
#version 420 core
uniform mat4 PROJECTION;
uniform mat4 VIEW;
layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec3 NORMAL;
in vec4 COLOR;
in vec2 UV;
out vec3 passNormal;
out vec4 passColor;
out vec2 passUV;
void main( void )
{
	vec4 local_pos = vec4( POSITION, 1 );
	//mat4 inv = inverse(VIEW);
	vec4 clip_pos =  PROJECTION * VIEW * local_pos;
	passColor = COLOR;
	passUV = UV;
	passNormal = NORMAL;
	gl_Position = clip_pos;
}
)";

const char* INVALID_FS = R"(
#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec3 passNormal;
in vec4 passColor;
in vec2 passUV;
out vec4 outColor;
void main( void )
{
	outColor = vec4(1,0,1,1);
}
)";

