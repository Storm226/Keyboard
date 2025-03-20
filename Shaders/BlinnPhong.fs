#version 330 core 

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Directional_Light{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct Spot_Light{
    vec3 position;
    vec3 direction;

    float cutoff;
    float outer_cutoff;

     // Attenuation
    float constant;
    float linear;
    float quadratic; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calculateDirectionalLight(Directional_Light light, vec3 surface_normal, vec3 view_direction);
vec3 calculateSpotLight(Spot_Light light, vec3 surface_normal, vec3 fragment_position, vec3 view_direction);


// Uniforms
uniform Directional_Light d_light;
uniform Spot_Light s_light;
uniform Point_Light p_light;

uniform vec3 view_position;
uniform Material material;

// in
in vec3 fragment_position;
in vec2 TexCoord;
in vec3 Color;
in vec3 Normal;

// out
out vec4 FragColor;

void main()
{    
    
   vec3 surface_normal = normalize(Normal);
   vec3 view_direction = normalize(view_position - fragment_position);
   vec3 result;

   // Directional Lighting
   result = calculateDirectionalLight(d_light, surface_normal, view_direction);
   // Spot Lighting
   result += calculateSpotLight(s_light, surface_normal, fragment_position, view_direction);

   FragColor = vec4(result, 1.0f);
}
 
 //// Directional Light
vec3 calculateDirectionalLight(Directional_Light light, vec3 surface_normal, vec3 view_direction){
    vec3 light_direction = normalize(-light.direction);

    float diff = max(dot(surface_normal, light_direction), 0.0f);
    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    float spec = pow(max(dot(view_direction,reflection_direction),0.0f), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoord));
    return ambient + diffuse + specular;
}

//// Spot Light

vec3 calculateSpotLight(Spot_Light light, vec3 surface_normal, vec3 fragment_position, vec3 view_direction){
    vec3 light_direction = normalize(light.position - fragment_position);
    float diff = max(dot(surface_normal, light_direction), 0.0f);

    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);

    float distance = length(light.position - fragment_position);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cutoff- light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular) * attenuation * intensity;
}