#version 460 core

uniform vec3 k_d;       
uniform vec3 k_s;       
uniform vec3 light_clr;
uniform vec3 view_dir;  




in vec3 fragment_position;
in vec2 tex_eval;
in vec3 light_pos_out;

out vec4 FragColor;

uniform sampler2D normalMap;

void main() {
    
    vec3 s_n = normalize((texture(normalMap, tex_eval.xy)).xyz);

    vec3 l_dir = normalize(light_pos_out - fragment_position);
    vec3 v_dir = normalize(view_dir); // Ensure view direction is normalized

    // Diffuse component
    float diffuse = max(dot(s_n, l_dir), 0.0);
    vec3 diffuse_color = k_d * light_clr * diffuse;

    // Blinn-Phong Specular component
    vec3 half_vector = normalize(v_dir + l_dir); // Halfway vector
    float spec = pow(max(dot(s_n, half_vector), 0.0), 32); // Shininess of 32
    vec3 specular_color = k_s * light_clr * spec;

    // Ambient component
    vec3 ambient = k_d * vec3(0.2f,0.2f,0.2f);

    // Final color computation
    vec3 output_color = ambient + diffuse_color + specular_color;

    FragColor = vec4(output_color, 1);


    //FragColor =   texture(tex, tex_coord.xy);
}
