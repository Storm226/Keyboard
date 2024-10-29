#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex_Coordinate;


// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// In

// Out
out vec3 Color;
out vec2 TexCoord;
out vec3 Normal;
out vec3 fragment_Position;

void main()
{
    Color = vec3(1.0f, 1.0, 1.0f);
    // Transform the normal. Use the inverse transpose of the model matrix
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoord = aTex_Coordinate;

    vec3 FragPos = vec3(model * vec4(aPosition, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // bring the object into worldspace to do lighting calculations
    fragment_Position = vec3(model * vec4(aPosition, 1.0f));

}