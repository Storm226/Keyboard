#version 330 core

// Uniforms
uniform mat4 V;
uniform float aspectRatio;

// Input/Output:
in vec2 gl_FragCoord;
out vec4 FragColor;

struct Ray {
    vec3 ro; // ray origin
    vec3 rd; // ray direction
};

struct PBRMat {
    vec3 color;
    float roughness;
    float metallic;
    float ao;
};

struct HitSurface {
    vec3 hit_point;
    vec3 normal;
    PBRMat material;
};

struct ISObj {
    float t;
    int i;
    float d;
};

uniform PBRMat pbr_mat[10]; // Define the material array

vec3 trace(in Ray r) {
    vec3 accum = vec3(0.0f);
    vec3 mask = vec3(1.0f);
    int nb_refl = 2; // Bounce number
    float c_refl = 1.0f;
    Ray curr_ray = r;

    for (int i = 0; i <= nb_refl; i++) {
        ISObj io = intersectObjects(curr_ray); // Ensure this function is defined somewhere
        if (io.t >= 0) {
            PBRMat mat = pbr_mat[io.i];

            HitSurface hs = HitSurface(curr_ray.ro + io.d * curr_ray.rd, computeNormal(io, curr_ray), mat);

            vec3 color = directIllumination(hs, curr_ray, c_refl);
            accum = accum + mask * color;
            mask = mask * c_refl;
            curr_ray = Ray(hs.hit_point + 0.001 * hs.normal, reflect(curr_ray.rd, hs.normal));
        } else {
            break;
        }
    }

    return accum;
}

vec3 directIllumination(in HitSurface hit, in Ray r, inout float refl) {
    vec3 color = vec3(0.0);
    for (int i = 0; i < light_nbr; i++) {
        Ray l_ray = lightRay(hit.hit_point, lights[i]); // Ensure `lightRay` and `lights` are defined
        l_ray.ro = hit.hit_point + 0.001 * hit.normal;
        ISObj io = intersectObjects(l_ray); // Placeholder for intersectObjects function
        float d_light = lightDist(hit.hit_point, lights[i]);

        if (io.t < 0 || (io.t >= 0 && (io.d >= d_light))) {
            color += PBR(hit, r, lights[i]); // Ensure `PBR` function and `lights` are defined
        } else {
            color += vec3(0.03) * hit.material.color * hit.material.ao;
        }

        vec3 Ve = normalize(r.ro - hit.hit_point);
        vec3 H = normalize(Ve + l_ray.rd);
        refl = length(fresnelSchlick(max(dot(H, Ve), 0.0), mix(vec3(0.04), hit.material.color, hit.material.metallic))) * hit.material.ao;
    }

    return color;
}

Ray generatePerspectiveRay(in vec2 p) {
    float fov = 30.0; // Half angle
    float D = 1.0 / tan(radians(fov));
    mat4 inv_view = inverse(V); // Get the matrix of the trackball

    vec3 up = vec3(0, 1, 0);
    vec3 front = vec3(0, 0, -1);
    vec3 right = cross(up, front);
    return Ray((inv_view * vec4(0, 0, -D, 1)).xyz, mat3(inv_view) * normalize(p.x * right + p.y * up * aspectRatio + D * front));
}

void main() {
    vec2 fragCoord = (gl_FragCoord.xy / vec2(800.0, 600.0)) * 2.0 - 1.0; // Assuming a resolution of 800x600
    Ray ray = generatePerspectiveRay(fragCoord);
    FragColor = vec4(trace(ray), 1.0);
}
