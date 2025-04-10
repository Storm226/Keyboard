#version 410 core

out vec4 color;

in vec2 texCoord;
in vec3 cameraPos;

uniform vec3 lightPosWorld;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform mat3 View;

uniform sampler2D normalMap;

void main() {
    vec3 normalColor = texture(normalMap, texCoord).rgb;
    vec3 lightPos = vec3(View * lightPosWorld);

    // ambient
    float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor * diffuseColor;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(normalColor);
    vec3 lightDir = normalize(lightPos - cameraPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(0 - cameraPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;

    //color = vec4(result, 1.0);
    //color = vec4(normalColor, 1.0);
    color = vec4(1, 0, 0, 1);
}

//Vertex
#version 410 core

layout(location = 0) in vec3 planePOS;
layout(location = 1) in vec2 planeTexCoord;

uniform mat4 planeMVP;

out vec3 modelPos;
out vec2 tesTextureCoordOut;

void main() {
    tesTextureCoordOut = planeTexCoord;
    modelPos = planePOS;
    tesTextureCoordOut.y = -tesTextureCoordOut.y;
    gl_Position = planeMVP * vec4(planePOS, 1.0);
}