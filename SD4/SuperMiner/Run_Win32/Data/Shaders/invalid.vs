// define the shader version (this is required)
#version 420 core

// Attributes - input to this shasder stage (constant as far as the code is concerned)
layout(location = 0) in vec3 POSITION;

// Entry point - required.  What does this stage do?
void main( void )
{
   // for now, we're going to set the 
   // clip position of this vertex to the passed 
   // in position. 
   // gl_Position is a "system variable", or have special 
   // meaning within the shader.
   gl_Position = vec4( POSITION, 1 ); 
}

