#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec2 iResolution; // Window resolution
uniform float iTime;      // Time for animation

// Ray direction
vec3 getRayDir(vec2 uv) {
    vec3 rayOrigin = vec3(0.0, 0.0, 1.0);  // Camera at z=1.0
    vec3 target = vec3(uv, 0.0);           // Target plane at z=0.0
    return normalize(target - rayOrigin);
}

// Signed Distance Function for a plane at z = 0
float sdfPlane(vec3 p) {
    return p.z;  // Distance from a point to the z=0 plane
}

// Raymarching function
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;  // Start at t = 0 (camera position)
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + t * rd;  // March forward
        float d = sdfPlane(p); // Distance from point to plane
        if (d < 0.001) break;  // Stop if we're very close to the surface
        t += d * 0.9;          // Move forward
    }
    return t;  // Return the distance t to the surface
}

void main()
{
    // Normalize screen coordinates
    vec2 uv = (TexCoord * 2.0 - 1.0) * vec2(iResolution.x / iResolution.y, 1.0);
    
    // Get ray direction
    vec3 rayDir = getRayDir(uv);
    
    // Ray origin
    vec3 rayOrigin = vec3(0.0, 0.0, 1.0);  // Camera at z = 1
    
    // Perform raymarching
    float dist = raymarch(rayOrigin, rayDir);
    
    // Color based on distance
    vec3 color = vec3(1.0 - dist * 0.1);  // Grayscale color
    
    FragColor = vec4(1.0, 0.0, 1.0, 1.0);  // Output final color
}
