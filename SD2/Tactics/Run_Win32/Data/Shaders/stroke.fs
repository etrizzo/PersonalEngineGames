#version 420 core
layout(binding = 0) uniform sampler2D gTexDiffuse;
in vec4 passColor;
in vec2 passUV;
out vec4 outColor;

const float xPixelDistance = .0008f;
const float yPixelDistance = .0008f;
const float jump = 1.f;
const float pointRange = 5.f;

void main( void )
{
	vec4 diffuse = texture( gTexDiffuse, passUV );

	vec2 point;
	outColor = diffuse * passColor;
  
 	 if (diffuse.a < .1f){
    	// Calculate the total color intensity around the pixel
    	// In this case we are calculating pixel intensity around 10 pixels
    	for(float u = -pointRange; u < pointRange ; u+=jump) {
   	     	for(float v = -pointRange ; v < pointRange ; v+=jump) {
    	        point.x = passUV.x  + u * xPixelDistance;
    	        point.y = passUV.y  + v * yPixelDistance;
    	         
    	        // If the point is within the range[0, 1]
    	        if (point.y > 0.0f && point.x > 0.0f &&
    	            point.y < 1.0f && point.x < 1.0f ) {
    	            vec4 samplePoint = texture(gTexDiffuse, point.xy);
       	     		if (samplePoint.a > .1f){
       	     			outColor = vec4(0,0,0,1);
            		}
                
            	}
        	}
    	}
    }

	
}








