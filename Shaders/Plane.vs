#version 460 core


out vec3 WorldPosition;

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 cameraWorldPosition;

const vec3 Pos[4] = vec3[](
    vec3(-1.0, 0.0, -1.0),  // Bottom Left
    vec3( 1.0, 0.0, -1.0),  // Bottom Right
    vec3( 1.0, 0.0,  1.0),  // Top Left
    vec3(-1.0, 0.0,  1.0)   // Top Right (no trailing comma!)
);

const int Indices[6] = int[](
    0, 2, 1,
    2, 0, 3
);

void main() {
    int Index = Indices[gl_VertexID];
    vec3 vPos = Pos[Index];

    vPos.x += cameraWorldPosition.x;
    vPos.z += cameraWorldPosition.z;

    vec4 vPos4 = vec4(vPos, 1.0);

    
    WorldPosition = vPos;

    gl_Position = Projection * View * vPos4;



}
