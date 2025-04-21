#version 460 core

out vec4 color;

in vec2 texCoord;
//in vec3 normal;
in vec3 fragPosWorld;

uniform vec3 cameraWorldPosition;
uniform vec3 lightPosWorld;
uniform vec3 lightColor;
uniform vec3 objectColor;

//uniform sampler2D reflectionMap;
//uniform samplerCube skybox;

void main() {
    //vec3 norm = normalize(normal);
    //vec3 cameraPos = cameraWorldPosition;
    //vec3 lightPos = lightPosWorld;

    //// ambient
    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor;

    //// diffuse 
    //vec3 lightDir = normalize(lightPos - fragPosWorld);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = diff * lightColor;
    
    //// specular - blinn-phong
    //float specularStrength = 0.5;
    //vec3 viewDir = normalize(cameraPos - fragPosWorld);
    //vec3 halfDir = normalize(lightDir + viewDir);

    //float spec = pow(max(dot(norm, halfDir), 0.0), 32.0); 
    //vec3 specular = specularStrength * spec * lightColor;  
        
    //vec3 result = (ambient + diffuse + specular) * objectColor;

    //// fresnel
    //float fresnel = pow(1.0 - clamp(dot(viewDir, norm), 0.0, 1.0), 5.0);

    //color = vec4(result, 1.0);
    color = vec4(1, 1, 1, 1);
}