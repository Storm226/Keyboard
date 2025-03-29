#version 460 core

layout (location = 0) in vec3 aPos;  // Vertex position input
layout (location = 1) in vec3 norm;  // Example: texture coordinates (if needed)
layout (location = 2) in vec3 tex;  // Example: texture coordinates (if needed)

out vec3 vPos;  // Pass position to geometry shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vPos = aPos;
}