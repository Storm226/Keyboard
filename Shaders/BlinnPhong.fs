#version 330 core 


struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
uniform vec3 light_color;

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
     //-----------------
    vec3 ambient = material.ambient * light.ambient;

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
    vec3 diffuse = material.diffuse * light.diffuse * diff;

    // Specular Lighting
    // -----------------
    vec3 view_direction = view_position - fragment_Position;
    view_direction = normalize(view_direction);
    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    float spec = pow(max(dot(view_direction, reflection_direction),0.0f), material.shininess);
    vec3 specular = material.specular * spec * light.specular;
    

    vec3 result = (ambient + diffuse + specular) * object_color * light.color;
    FragColor = vec4(result, 1.0f);   
    
    //FragColor = vec4(vec3(gl_FragCoord.z * gl_FragCoord.z, 0.0f, 0.0f), 1.0f);

    // if texturing set true
    if(false){
     vec4 blendedTexture = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

    // Apply lighting to the RGB channels, keep alpha from texture
    vec3 resultColor = (ambient + diffuse) * blendedTexture.rgb;
    FragColor = vec4(resultColor, blendedTexture.a);
    }


}
 