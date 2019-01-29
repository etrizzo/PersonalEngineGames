// define the shader version (this is required)
#version 420 core

// Create a "uniform", a variable that is constant/uniform 
// for the entirety of the draw call.  This one will
// store our projection, or "ViewToClip" matrix.
uniform mat4 PROJECTION; // new

// Attributes - input to this shasder stage (constant as far as the code is concerned)
layout(location = 0) in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;

out vec2 passUV; 
out vec4 passColor;
  

void main() 
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * local_pos; 

   passUV = UV; // we have to set our outs.
   passColor = COLOR;
   gl_Position = clip_pos; 
}

