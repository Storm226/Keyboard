#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 mvp;
uniform mat4 normalMatrix;
uniform mat4 modelView;
uniform vec3 lightPosWorld;
uniform mat3 View;

out vec3 FragPos;  
out vec3 normalColor;
out vec3 lightPos;

void main() {
    FragPos = vec3(modelView * vec4(pos, 1.0));
    lightPos = vec3(View * lightPosWorld);
    normalColor = normalize(vec3(transpose(inverse(modelView)) * vec4(normal, 1.0)));

    gl_Position = mvp * vec4(pos, 1.0); 
}