#version 460 core

// Uniforms
uniform vec3 k_d;       // Diffuse reflectance
uniform vec3 k_s;       // Specular reflectance
uniform vec3 light_pos; // Light position
uniform vec3 light_clr; // Light color
uniform vec3 view_dir;  // View direction

uniform sampler2D depthMap;

// Inputs from vertex shader
in vec3 surface_normal;
in vec3 fragment_position;
in vec3 light_pos_out;

in vec4 lightView_Position;


// Output color
out vec4 FragColor;

void main() {
    // Normalize input vectors
    vec3 s_n = normalize(surface_normal);
    vec3 l_dir = normalize(light_pos_out - fragment_position);
    vec3 v_dir = normalize(view_dir); 

    // Diffuse component
    float diffuse = max(dot(s_n, l_dir), 0.0);
    vec3 diffuse_color = k_d * light_clr * diffuse;

    // Blinn-Phong Specular component
    vec3 half_vector = normalize(v_dir + l_dir); 
    float spec = pow(max(dot(s_n, half_vector), 0.0), 32); 
    vec3 specular_color = k_s * light_clr * spec;

    // Ambient component
    vec3 ambient = k_d * vec3(0.2f,0.2f,0.2f);

    // Final color computation
    vec3 output_color = ambient + diffuse_color + specular_color;

    // convert the 4d lightViewPositoin to 3d
    vec3 p = lightView_Position.xyz / lightView_Position.w;

   // ALEX i know im getting some seemingly valid values for p
   //FragColor = vec4(p.x, p.y, 0, 1);

   float currentDepth = p.z;
   float closestDepth = texture(depthMap, p.xy).r;

   float bias = 0.0005;
   //float bias = max(0.0005 * (1.0 - dot(s_n, l_dir)), 0.005);  
   float shadow = currentDepth - bias < closestDepth  ? 1.0 : 0.0;      
    

    FragColor = vec4(output_color,1) * shadow;

}
