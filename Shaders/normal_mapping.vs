#version 460 core

layout  (location = 0) in vec3 v_position;
layout  (location = 1) in vec3 v_normal;
layout  (location = 2) in vec3 v_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 light_pos; 


out vec3 tex_coord;
out vec3 fragment_position;
out vec3 light_pos_out;

void main(){

    // for shading calculation
    vec4 w_pos = view * model * vec4(v_position, 1.0);
    fragment_position = w_pos.xyz;

    vec4 l_pos = view * model * vec4(light_pos, 1.0);
    light_pos_out = l_pos.xyz;

    tex_coord = v_tex;
    gl_Position = projection * view * model * vec4(v_position, 1.0);
}