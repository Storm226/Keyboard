#version 460 core


layout  (location = 0) in vec3 v_position;
layout(location = 1) in vec2 textureCoords;


uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 projector_view;
uniform mat4 range;
uniform float time;


uniform float amplitude;





vec3 getDisplacedNormal(vec2 uv);
vec2 fade(vec2 t);
vec4 permute(vec4 x);
float perlinNoise(vec2 P);
vec3 getDisplacedPosition(vec2 uv);
bool intersectSegmentWithXZPlane(vec3 p0, vec3 p1, float planeY, out vec3 contactPoint);

out vec2 texCoord;
out vec3 fragPosWorld;
out vec3 normal;


void main() {

   mat4 M_Projector = inverse(camera_projection * projector_view);


   // uncomment this line if you want to have a bad time
   //mat4 M_Projector = inverse(camera_projection * projector_view) * range;

    // z = 1
    vec4 temp1 = M_Projector * vec4(v_position.x, v_position.y, 1.0, 1.0);
    vec3 p1 = temp1.xyz / temp1.w;

    // z = -1
    vec4 temp2 = M_Projector * vec4(v_position.x, v_position.y, -1.0, 1.0);
    vec3 p2 = temp2.xyz / temp2.w;


    vec3 contact;
    if (intersectSegmentWithXZPlane(p1, p2, 1.0f, contact)) {
    // Generate noise-based displacement
    float displacement = perlinNoise((contact.xz + vec2(time * 0.2)) * 0.5);

    // Apply the noise as vertical displacement
    contact.y += displacement * amplitude; // Adjust amplitude (0.2) as needed
        fragPosWorld = contact;
        normal = getDisplacedNormal(contact.xz);
        gl_Position = camera_projection * camera_view * vec4(contact, 1.0);

    } else {
        fragPosWorld = contact;
        normal = getDisplacedNormal(contact.xz);
        gl_Position = camera_projection * camera_view * vec4(0, 0, 0, 1);
    }

    texCoord = vec2(contact.x, contact.y);

}


bool intersectSegmentWithXZPlane(vec3 p1, vec3 p2, float planeY, out vec3 contactPoint) {
    vec3 n = vec3(0.0, 1.0, 0.0); // XZ plane normal
    vec3 p0 = vec3(0.0, planeY, 0.0); // A point on the XZ plane

    vec3 dir = p2 - p1; // direction of the segment
    float denom = dot(dir, n);

    // Check if line is parallel to the plane
    if (abs(denom) < 1e-6)
        return false;

    float t = dot(p0 - p1, n) / denom;

    if (t >= 0.0 && t <= 1.0) {
        contactPoint = p1 + t * dir;
        return true;
    }

    return false;
}



// GLSL Classic Perlin 2D Noise
vec4 permute(vec4 x) {
    return mod(((x*34.0)+1.0)*x, 289.0);
}

vec2 fade(vec2 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float perlinNoise(vec2 P) {
    vec4 Pi = floor(vec2(P.x, P.y).xyxy) + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 Pf = fract(vec2(P.x, P.y).xyxy) - vec4(0.0, 0.0, 1.0, 1.0);

    Pi = mod(Pi, 289.0); // To avoid wrapping
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);

    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;

    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);

    vec4 norm = 1.79284291400159 - 0.85373472095314 * 
        vec4(dot(g00,g00), dot(g10,g10), dot(g01,g01), dot(g11,g11));
    g00 *= norm.x;
    g10 *= norm.y;
    g01 *= norm.z;
    g11 *= norm.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    vec2 fade_xy = fade(vec2(Pf.x, Pf.y));
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);

    return 2.3 * n_xy;
}

vec3 getDisplacedPosition(vec2 uv) {
    float freq = 1.0;
    float amp = 1.0;
    float alpha = 0.5;
    float displacement = 0.0;

    for (int i = 0; i < 5; ++i) {
        displacement += perlinNoise((uv + time * 0.1) * freq) * amp;
        freq *= 2.0;
        amp *= alpha;
    }

    return vec3(uv.x, displacement * 0.2, uv.y); // note: uv = xz
}

vec3 getDisplacedNormal(vec2 uv) {
    float epsilon = 0.01;

    vec3 center = getDisplacedPosition(uv);
    vec3 offsetX = getDisplacedPosition(uv + vec2(epsilon, 0.0));
    vec3 offsetZ = getDisplacedPosition(uv + vec2(0.0, epsilon));

    vec3 tangentX = offsetX - center;
    vec3 tangentZ = offsetZ - center;

    vec3 normal = normalize(cross(tangentZ, tangentX));
    return normal;
}
