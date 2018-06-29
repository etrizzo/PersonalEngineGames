const int MAX_LIGHTS = 8;

//LIGHTING ============================================
 

// Scene related
uniform vec4 AMBIENT; // xyz color, w intensity



// surface 
uniform float SPECULAR_AMOUNT; // shininess (0 to 1)
uniform float SPECULAR_POWER; // smoothness (1 to whatever)


struct light_t 
{
   vec4 LIGHT_COLOR;

   vec3 LIGHT_POSITION;
   float USES_SHADOWS;
   
   vec3 LIGHT_FORWARD_DIR;
   float IS_POINT_LIGHT;
   
   vec3 LIGHT_ATTENUATION;
   float DOT_INNER_ANGLE;
   
   vec3 SPECULAR_ATTENUATION;
   float DOT_OUTER_ANGLE;

   mat4 SHADOW_VP;
}; 


struct light_factor_t 
{
   vec3 diffuse; 
   vec3 specular; 
}; 

layout(binding=2, std140) uniform uboLightBuffer 
{
   light_t LIGHTS[MAX_LIGHTS];
};


float GetAttenuation( float i, float d, vec3 a )
{
   return i / (a.x + d * a.y + d * d * a.z);
   //return i / (d * d);
}


//--------------------------------------------------------------------------------------
light_factor_t CalculateLightFactor( vec3 position, 
   vec3 eye_dir, 
   vec3 normal, 
   light_t light, 
   float spec_factor, 
   float spec_power )
{
   light_factor_t lf; 

   vec3 light_color = light.LIGHT_COLOR.xyz;

   // get my direction to the light, and distance
   vec3 light_dir = light.LIGHT_POSITION - position; // direction TO the light
   float dist = length(light_dir); 
   light_dir /= dist; 

   // 
   vec3 light_forward = normalize(light.LIGHT_FORWARD_DIR); 

   // get the power
   float light_intensity = light.LIGHT_COLOR.w; 

   // figure out how far away angle-wise I am from the forward of the light (useful for spot lights)
   float dot_angle = dot( light_forward, -light_dir ); 

   // falloff for spotlights.
   float angle_attenuation = smoothstep( light.DOT_OUTER_ANGLE, light.DOT_INNER_ANGLE, dot_angle ); 
   light_intensity = light_intensity * angle_attenuation; 

   // get actual direction light is pointing (spotlights point in their "forward", point lights point everywhere (ie, toward the point))
   light_dir = mix( -light_forward, light_dir,light.IS_POINT_LIGHT);

   float attenuation = clamp( GetAttenuation( light_intensity, dist, light.LIGHT_ATTENUATION ), 0, 1 ); 
   float spec_attenuation = clamp( GetAttenuation( light_intensity, dist, light.SPECULAR_ATTENUATION ), 0, 1 );   //further falloff because shiny shit is cool


   //===== CALCULATE DIFFUSE FOR THE LIGHT =======
   // Finally, calculate dot3 lighting
   float dot3 = dot( light_dir, normal ); 
   float diffuse_factor = clamp( attenuation * dot3, 0.0f, 1.0f );

   
   //===== CALCULATE SPECULAR FOR THE LIGHT =======
   vec3 r = reflect(-light_dir, normal); 
   float spec_amount = max(dot(r, eye_dir), 0.0f); 
   float spec_intensity = (spec_attenuation * spec_factor) * pow(spec_amount, spec_power); 

   lf.diffuse = light_color * diffuse_factor;
   lf.specular = light_color * spec_intensity; 

   return lf; 
}



light_factor_t CalculateLighting( vec3 world_pos, 
   vec3 eye_dir, 
   vec3 normal, 
   float spec_factor, 
   float spec_power ) 
{
   light_factor_t lf; 
   lf.diffuse = AMBIENT.xyz * AMBIENT.w; 
   lf.specular = vec3(0.0f); 

   spec_factor *= spec_factor; 
   spec_power *= spec_power; 

   for (uint i = 0; i < MAX_LIGHTS; ++i) {
      light_factor_t l = CalculateLightFactor( world_pos, eye_dir, normal, LIGHTS[i], spec_factor, spec_power ); 
      lf.diffuse += l.diffuse;
      lf.specular += l.specular; 
   }

   lf.diffuse = clamp( lf.diffuse, vec3(0.0f), vec3(1.0f) ); 
   return lf; 
}