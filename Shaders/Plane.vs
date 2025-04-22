#version 460 core


layout  (location = 0) in vec3 v_position;

uniform mat4 camera_view;
uniform mat4 camera_projection;
uniform mat4 projector_view;



bool intersectSegmentWithXZPlane(vec3 p0, vec3 p1, float planeY, out vec3 contactPoint);

void main() {

   mat4 M_Projector = inverse(camera_projection * projector_view);

    // z = 1
    vec4 temp1 = M_Projector * vec4(v_position.x, v_position.y, 1.0, 1.0);
    vec3 p1 = temp1.xyz / temp1.w;

    // z = -1
    vec4 temp2 = M_Projector * vec4(v_position.x, v_position.y, -1.0, 1.0);
    vec3 p2 = temp2.xyz / temp2.w;


    vec3 contact;
    if(intersectSegmentWithXZPlane(p1, p2, 1.0f, contact ))
        gl_Position = camera_projection * camera_view * vec4(contact, 1.0);
    else
        gl_Position = camera_projection * camera_view * vec4(0,0,0,1);

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
