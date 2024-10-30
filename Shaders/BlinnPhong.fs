#version 330 core 


struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


// Uniforms
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 object_color;

uniform vec3 view_position;

uniform Material material;
uniform Light light;

// in
in vec2 TexCoord;
in vec3 Color;
in vec3 Normal;
in vec3 fragment_Position;

// out
out vec4 FragColor;




void main()
{    
    
    // Ambient lighting
    // ----------------
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // Diffuse lighting
    //-----------------
    // calculate and normalize direction vector
    vec3 surface_normal = normalize(Normal);
    vec3 light_direction = light.position - fragment_Position;
    light_direction = normalize(light_direction);

    // V dot W == 0 if perpendicular
    // > 0 if acute angle between 
    // < 0 if obtuse
    float diff = max(dot(surface_normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // Specular Lighting
    // -----------------
    vec3 view_direction = view_position - fragment_Position;
    view_direction = normalize(view_direction);
    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    float spec = pow(max(dot(view_direction, reflection_direction),0.0f), material.shininess);
    vec3 specular =  vec3(texture(material.specular, TexCoord)) * spec * light.specular;
    

    vec3 result = (ambient + diffuse + specular) * light.color;
    FragColor = vec4(result, 1.0f);   
  
}
 