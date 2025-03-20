

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Color;

uniform sampler2D texture_diffuse1;

void main()
{    
    //FragColor = texture(texture_diffuse1, TexCoords) * vec4(.5f, .5f, .5f, 1.0f);
    FragColor = vec4(.5f, .5f, .5f, 1.0f);
}

