#version 460 core

layout  (location = 0) in vec3 v_position;
layout  (location = 1) in vec3 v_normal;
layout  (location = 2) in vec3 v_tex;

uniform mat4 mvp;

void main(){

    // set the position to be in the canonical view volume
    gl_Position = mvp * vec4(v_position, 1.0);
    
    }