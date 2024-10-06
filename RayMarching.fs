#version 330 core

// Uniforms
uniform mat4 view;

// Input/Output:
in vec2 gl_FragCoord;

out vec4 FragColor;


struct PBRMat 
{
    vec3 color;
    float roughness;
    float metallic;
    float ao;
};

struct hitSurface
{
   vec3 hitPoint;
   vec3 normal;
   PBRMat material;
};

vec3 trace(in Ray r)
{
    vec3 accum = vec3(0.0f);        // color of pixel incremented at each bounce
    vec3 mask = vec3(1.0f);         // intensity of current ray which decreases at each bounce
    int nb_refl = 2;                // bounce number
    float c_refl = 1.0f;
    Ray current_ray = r;

    for(int i = 0; i <= nb_refl; i++)
    {
        ISObj io = intersectObjects(current_ray);
        if (io.t >= 0)
        {
            PBRMat mat = pbr_mat[io.i];

            hitSurface hs = hitSurface(current_ray.ro + io.d * current_ray.rd, computeNormal(io, current_ray), mat);

            vec3 color = directIllumination(hs, current_ray, c_refl);
            accum += mask * color;
            mask = mask*c_refl;
            current_ray = Ray(hs.hit_point + 0.001*hs.normal, reflect(current_ray.rd, hs.normal));
        }
        else{
            break;
        }
        return accum;
    }
}



Ray generatePerspectiveRay(in vec2 pointOnSurface)
{
    // pointOnSurface is the current pixel Coord in [-1,1] ie cannonical view volume
    float fov = 30;
    float D = 1./tan(radians(fov));  // represents distance from camera to near clipping plane
    mat4 inverseView = inverse(V); 

    vec3 Up = vec3(0,1,0);
    vec3 front = vec3(0,0,-1);
    vec3 right = cross(up,front);
    return  Ray((inv_view*vec4(0,0,-D,1)).xyz,mat3(inv_view)*normalize(p.x*right + p.y*up*aspectRatio + D*front));
}

void main()
{

    fragCoord = gl_FragCoord;


    Ray ray = generatePerspectiveRay(fragCoord);

    FragColor = vec4( trace(ray),1);
    // FragColor = vec4(0.5, .5, .8, 1);

}
