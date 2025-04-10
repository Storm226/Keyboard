#version 410 core

out vec4 color;

in vec2 texCoord;
in vec3 fragPosWorld;
in vec3 cameraPos;

uniform vec3 lightPosWorld;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform mat3 View;

uniform sampler2D normalMap;
uniform sampler2D reflectionMap;

void main() {
    vec3 normalColor = texture(normalMap, texCoord).rgb;
    vec3 norm = normalize(normalColor);

    vec3 lightPos = vec3(View * lightPosWorld);

    // ambient
    float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor * diffuseColor;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 lightDir = normalize(lightPos - fragPosWorld);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular - blinn-phong
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - fragPosWorld);
    vec3 halfDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0) //shiniess is 3
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;

    // fresnel
    float fresnel = pow(1.0 - clamp(dot(viewDir, normal), 0.0, 1.0), 5.0);

    //disort reflection w normal
    vec2 distortion = norm.xz * 0.02;
    vec2 distortionUV = texCoord + distortion;
    vec3 reflectionColor = texture(reflectionMap, distortionUV).rgb;

    vec3 finalcolor = mix(litColor, reflectionColor, fresnel);

    //color = vec4(finalcolor, 1.0);
    color = vec4(1, 0, 0, 1);
}