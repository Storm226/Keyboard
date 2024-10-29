#version 330 core 

// uniform
uniform sampler2D texture1;
uniform sampler2D texture2;
  
uniform vec3 object_color;
uniform vec3 light_color;

uniform vec3 light_position;
uniform vec3 view_position;

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
    float ambient_str = 0.5f;
    vec3 ambient = ambient_str * light_color;

    // Diffuse lighting
    //-----------------
    // calculate and normalize direction vector
    vec3 surface_normal = normalize(Normal);
    vec3 light_direction = light_position - fragment_Position;
    light_direction = normalize(light_direction);

    // V dot W == 0 if perpendicular
    // > 0 if acute angle between 
    // < 0 if obtuse
    float diff = max(dot(surface_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    // Specular Lighting
    // -----------------
    float specular_str = .5f;
    vec3 view_direction = view_position - fragment_Position;
    view_direction = normalize(view_direction);
    vec3 reflection_direction = reflect(-light_direction, surface_normal);
    int shinyness = 32; // higher shineness, smaller highlight because more properly reflects light
    float spec = pow(max(dot(view_direction, reflection_direction),0.0f), shinyness);
    vec3 specular = specular_str * spec * light_color;
    

    vec3 result = (ambient + diffuse + specular) * object_color;
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
 