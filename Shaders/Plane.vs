#version 460 core

out vec3 position;
uniform vec3 cameraWorldPosition;

const vec3 Pos[4] = vec3[](
    vec3(-1.0, 0.0, -1.0),  // Bottom Left
    vec3( 1.0, 0.0, -1.0),  // Bottom Right
    vec3( 1.0, 0.0,  1.0),  // Top Right
    vec3(-1.0, 0.0,  1.0)   // Top Left
);

void main() {


    vec3 vPos = Pos[gl_VertexID] * vec3(3.0, 1.0, 3.0);
  //  vPos.x += cameraWorldPosition.x;
   // vPos.z += cameraWorldPosition.z;
    position = vPos;
    gl_Position = vec4(vPos, 1.0);
}