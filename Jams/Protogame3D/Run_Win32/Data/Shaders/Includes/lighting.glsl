const int MAX_LIGHTS = 8;

//LIGHTING ============================================
 

// Scene related
uniform vec4 AMBIENT; // xyz color, w intensity



// surface 
uniform float SPECULAR_AMOUNT; // shininess (0 to 1)
uniform float SPECULAR_POWER; // smoothness (1 to whatever)


//--------------------------------------------------------------------------------------
// if using shadow; 
layout(binding = 8) uniform sampler2D gTexShadow; 

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
// return 1 if fully lit, 0 if should be fully in shadow (ignores light)
float GetShadowFactor( vec3 position, vec3 normal, light_t light )
{
   float shadow = light.USES_SHADOWS;
   if (shadow == 0.0f) {
      return 1.0f; 
   }

   // so, we're lit, so we will use the shadow sampler
   float bias_factor = max( dot( light.LIGHT_FORWARD_DIR, normal ), 0.0f ); 
   bias_factor = sqrt(1 - (bias_factor * bias_factor)); 
   position -= light.LIGHT_FORWARD_DIR * bias_factor * .25f; 

   vec4 clip_pos = light.SHADOW_VP * vec4(position, 1.0f);
   vec3 ndc_pos = clip_pos.xyz / clip_pos.w; 

   // put from -1 to 1 range to 0 to 1 range
   ndc_pos = (ndc_pos + vec3(1)) * .5f;
   
   // can give this a "little" bias
   // treat every surface as "slightly" closer"
   // returns how many times I'm pass (GL_LESSEQUAL)
   float depthAtPosition = texture( gTexShadow, ndc_pos.xy ).r; 
   float my_depth = ndc_pos.z; 
   
   // use this to feathre shadows near the border
   float min_uv = min( ndc_pos.x, ndc_pos.y ); 
   float max_uv = max( ndc_pos.x, ndc_pos.y ); 
   float blend = 1.0f - min( smoothstep(0.0f, .05f, min_uv), smoothstep(1.0, .95, max_uv) ); 

   // step returns 0 if nearest_depth is less than my_depth, 1 otherwise.
   // if (nearest_depth) is less (closer) than my depth, that is shadow, so 0 -> shaded, 1 implies light
   float is_lit = step( my_depth, depthAtPosition ); // 
   float litDiff = depthAtPosition - my_depth;
   float litFactor = litDiff;
   clamp(litFactor, 0.0, 1.0);

   // scale by shadow amount
  return mix( light.USES_SHADOWS * is_lit, 1.0f, blend ); 
  //return light.USES_SHADOWS * litFactor; 
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

   float shadowing = GetShadowFactor( position, normal, light ); 

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

   lf.diffuse = shadowing * light_color * diffuse_factor;
   lf.specular = shadowing * light_color * spec_intensity; 

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



//--------------------------------------------------------------------------------------
light_factor_t CalculateLightFactorCelShaded( vec3 position, 
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

   float shadowing = GetShadowFactor( position, normal, light ); 

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

   if (diffuse_factor <= .2f)
   {
      diffuse_factor = 0.0f;
   } else if (diffuse_factor <= .5f)
   {
      diffuse_factor = 0.5f;
   } else {
      diffuse_factor = 1.0f;
   }

   
   //===== CALCULATE SPECULAR FOR THE LIGHT =======
   vec3 r = reflect(-light_dir, normal); 
   float spec_amount = max(dot(r, eye_dir), 0.0f); 
   float spec_intensity = (spec_attenuation * spec_factor) * pow(spec_amount, spec_power); 

   lf.diffuse = shadowing * light_color * diffuse_factor;
   lf.specular = shadowing * light_color * spec_intensity; 

   return lf; 
}


light_factor_t CalculateLightingCelShaded( vec3 world_pos, 
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
      light_factor_t l = CalculateLightFactorCelShaded( world_pos, eye_dir, normal, LIGHTS[i], spec_factor, spec_power ); 
      lf.diffuse += l.diffuse;
      lf.specular += l.specular; 
   }

   lf.diffuse = clamp( lf.diffuse, vec3(0.0f), vec3(1.0f) ); 
   return lf; 
}