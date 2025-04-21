#version 460 core

out vec3 position;

const vec3 Pos[4] = vec3[](
    vec3(-1.0, -1.0, 0.0),  // Bottom Left
    vec3( 1.0, -1.0, 0.0),  // Bottom Right
    vec3( 1.0, 1.0,  0.0),  // Top Right
    vec3(-1.0, 1.0,  0.0)   // Top Left
);





void main() {

    vec3 vPos = Pos[gl_VertexID];
    position = vPos;
    gl_Position = vec4(vPos, 1.0);
}