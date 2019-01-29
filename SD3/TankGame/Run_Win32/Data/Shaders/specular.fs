#version 420 core

// Uniforms ==============================================
// Constants
uniform vec3 EYE_POSITION;  // camera related

// Scene related
uniform vec4 AMBIENT; // xyz color, w intensity

// lighting - replace with uniform buffer someday
//uniform vec3 LIGHT_POSITION; 
//uniform vec4 LIGHT_COLOR; // xyz color, w intensity


// Textures
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal; 

layout(binding=1, std140) uniform uboTimeClock 
{
   float GAME_DELTA_TIME;
   float GAME_TIME; 
   float SYSTEM_DELTA_TIME; 
   float SYSTEM_TIME; 
}; 

// Attributes ============================================
in vec2 passUV; 
in vec4 passColor; 
in vec3 passWorldPos;   // new
in vec3 passNormal;
in vec3 passTangent;
in vec3 passBiTan;

out vec4 outColor; 



//LIGHTING ============================================

const int MAX_LIGHTS = 8;

// surface 
uniform float SPECULAR_AMOUNT; // shininess (0 to 1)
uniform float SPECULAR_POWER; // smoothness (1 to whatever)

struct light_t 
{
   vec4 LIGHT_COLOR;

   vec3 LIGHT_POSITION;
   float padding00;
   
   vec3 LIGHT_FORWARD_DIR;
   float IS_POINT_LIGHT;
   
   vec3 LIGHT_ATTENUATION;
   float DOT_INNER_ANGLE;
   
   vec3 SPECULAR_ATTENUATION;
   float DOT_OUTER_ANGLE;
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


// Entry Point ===========================================
void main( void )
{
   // Interpolation is linear, so normals become not normal
   // over a surface, so renormalize it. 
   //vec3 surface_normal = normalize(passNormal);

   // Color of this surface
   vec4 surface_color = texture( gTexDiffuse, passUV ); 
   vec4 normal_color= texture(gTexNormal, passUV);

   vec3 surface_light = vec3(0); // How much light is hitting the surface
   vec3 reflected_light = vec3(0);  // How much light is reflected back

   vec3 light_dir = normalize(LIGHTS[0].LIGHT_POSITION - passWorldPos); // direction to light
   vec3 eye_dir = normalize(EYE_POSITION - passWorldPos); // direction to the eye

   // *********
   //  AMBIENT
   // *********
   // first, add in Ambient light (general brightness of the room)
   // to our surface light
   // Ambient color * ambient intensity
   surface_light = AMBIENT.xyz * AMBIENT.w;



   //calculate world normal


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



   light_factor_t lf = CalculateLighting( passWorldPos, eye_dir, world_normal, SPECULAR_AMOUNT, SPECULAR_POWER ); 


   // Calculate final color; note, it should not change the alpha... so...
   // Alpha of one for surface light as it is multiplied in,
   // and 0 for specular since it is added in.
   //vec4 final_color = vec4(surface_light, 1) * surface_color + vec4(reflected_light, 0); 
   vec4 final_color = vec4(lf.diffuse, 1) * surface_color + vec4(lf.specular, 0); 

   // Reclamp so that we're in a valid colour range.  May want to save off overflow
   // if doing bloom.
   final_color = clamp(final_color, vec4(0), vec4(1) ); 
   
   // Output the colour
   outColor = vec4(lf.specular, 1.f);
}