#version 460 core

layout  (location = 0) in vec3 v_position;
layout  (location = 1) in vec3 v_tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec2 tex_coord;


out vec3 position;

void main(){



    
    position = v_position;
    tex_coord = v_tex.xy;
    gl_Position = vec4(v_position, 1.0);
}