#version 460 core


layout  (location = 0) in vec3 v_position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 projector_view;


bool intersectSegmentWithXZPlane(vec3 p0, vec3 p1, float planeY, out vec3 contactPoint) {
    vec3 dir = p1 - p0;

    // Avoid division by zero (segment is parallel to the XZ plane)
    if (abs(dir.y) < 1e-6) {
        return false;
    }

    float t = (planeY - p0.y) / dir.y;

    // Check if t is within the segment bounds
    if (t >= 0.0 && t <= 1.0) {
        contactPoint = p0 + t * dir;
        return true;
    }

    return false;
}


void main() {

    mat4 M_Projector = inverse(projection * projector_view);

    // z = 1
    vec3 p1 = (M_Projector * vec4(v_position.x, v_position.y, 1, 1)).xyz;

    // z = -1
    vec3 p2 = (M_Projector * vec4(v_position.x, v_position.y, -1, 1)).xyz;

    vec3 contact;
    if(intersectSegmentWithXZPlane(p1, p2, 0.0f, contact ))
        gl_Position = vec4(contact, 1.0);
    else
        gl_Position = vec4(0,0,0,1);

}