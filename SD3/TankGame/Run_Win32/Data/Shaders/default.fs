#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor;
in vec2 passUV; 

out vec4 outColor; 
  
void main() 
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   outColor = diffuse * passColor;
}