#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Headers/Shader.h"
#include "Headers/Camera.h"

#include "Headers/lodepng.h"
#include "Headers/definitions.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


                // position                     // up                   // pitch, yawS
Camera camera(glm::vec3(0.0f,20.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -0.0f, 0.0f); 
Camera projector(glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), -10.0f, -30.0f);
std::vector<glm::vec3> plane_vertices;


int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";

    Shader s("Shaders/Plane.vs", "Shaders/basic_object_shader.fs", nullptr, nullptr, nullptr);
    GLuint obj_VAO, obj_VBO;

    glm::mat4 view;
    glm::mat4 perspective;
    glm::mat4 projector_view;
    glm::mat4 camera_view_projection;
    glm::mat4 M_Projector;
    glm::mat4 range;
 

    setupPlaneMesh();
    populate_buffer(obj_VAO, obj_VBO, plane_vertices, false, false);
    std::vector<glm::vec3> intersections;

 
    glEnable(GL_DEPTH_TEST);

    s.use();

        //render loop
        // ----------------------------------
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



            if (!wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);


            // camera view and perspective projection matrices
            view = camera.GetViewMatrix();
            perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
            camera_view_projection = perspective * view;

            // projector matrices
            projector.Front = aimAtHorizon(camera.Front, projector.Position);
            projector.updateCameraVectors();
            projector_view = projector.GetViewMatrix();
            M_Projector = glm::inverse(perspective * projector_view);


            // trying to generate range matrix
            if (bool worldSpaceIntersection = checkWorldSpaceIntersection(intersections, camera_view_projection)) {

                project_onto_base_plane(intersections);

                range = generate_range_matrix(intersections, glm::inverse(M_Projector));
            }


            // uniform values needed for projected grid / perlin noise
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "camera_view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "camera_projection"), 1, GL_FALSE, glm::value_ptr(perspective));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "projector_view"), 1, GL_FALSE, glm::value_ptr(projector_view));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "range"), 1, GL_FALSE, glm::value_ptr(range));
            float time = glfwGetTime();
            glUniform1f(glGetUniformLocation(s.ID, "time"), time);



            s.use();
            draw(obj_VAO, plane_vertices);

            std::cout << "Camera Position : ";
            printVec(camera.Position);
            std::cout << "Projector Position : ";
            printVec(projector.Position);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    cleanUp();

    return 0;
}


void setupPlaneMesh() {


    
    plane_vertices.reserve((100 - 1) * (100 - 1) * 6); // 2 triangles per cell × 3 vertices

    const int gridSize = 100;
    const float step = 2.0f / (gridSize - 1); // from -1 to 1

    for (int y = 0; y < gridSize - 1; ++y) {
        for (int x = 0; x < gridSize - 1; ++x) {
            // Convert to normalized coordinates in [-1, 1]
            float x0 = -1.0f + x * step;
            float x1 = -1.0f + (x + 1) * step;
            float y0 = -1.0f + y * step;
            float y1 = -1.0f + (y + 1) * step;

            glm::vec3 topLeft = glm::vec3(x0, y0, 0.0f);
            glm::vec3 topRight = glm::vec3(x1, y0, 0.0f);
            glm::vec3 bottomLeft = glm::vec3(x0, y1, 0.0f);
            glm::vec3 bottomRight = glm::vec3(x1, y1, 0.0f);

            // Triangle 1: topLeft -> bottomLeft -> topRight
            plane_vertices.push_back(topLeft);
            plane_vertices.push_back(bottomLeft);
            plane_vertices.push_back(topRight);

            // Triangle 2: topRight -> bottomLeft -> bottomRight
            plane_vertices.push_back(topRight);
            plane_vertices.push_back(bottomLeft);
            plane_vertices.push_back(bottomRight);
        }
    }
}

// the role of this function is to generate a matrix which defines a transformation from (0, 1) onto the span of the 
// visible volume in screen space
glm::mat4 generate_range_matrix(std::vector<glm::vec3> intersections, glm::mat4 inverse_M_Projector) {

    // so now we wanna transform the points in the buffer to projector space by using inverse of M_projector
    // the x and y span of the visible volume is now the x/y spans of the points in the buffer

    float Xmin, Xmax, Ymin, Ymax;

    // we can do this transformation and also compute the minX/maxX minY/maxY
    for (int i = 0; i < intersections.size(); i++) {

        // in spirit we want the inverse, which should be the second line not the first as i had
         // intersections[i] = glm::vec4(intersections[i], 0) * glm::inverse(M_Projector);
        intersections[i] = inverse_M_Projector * glm::vec4(intersections[i], 1);
        if (i == 0) {
            Xmin = intersections[i].x;
            Xmax = intersections[i].x;
            Ymin = intersections[i].y;
            Ymax = intersections[i].y;

            continue;
        }

        if (intersections[i].x < Xmin)
            Xmin = intersections[i].x;
        else if (intersections[i].x > Xmax)
            Xmax = intersections[i].x;

        // z span
        if (intersections[i].y < Ymin)
            Ymin = intersections[i].y;
        else if (intersections[i].y > Ymax)
            Ymax = intersections[i].y;
    }


    // define the range matrix to augment the m_projector matrix
    glm::mat4 range = glm::mat4(1.0);
    range[0][0] = Xmax - Xmin;
    range[3][0] = Xmin;
    range[1][1] = Ymax - Ymin;
    range[3][1] = Ymin;

    return range;
}

// the paper says we need to project world space positions onto the s_base plane
// i dont remember why right now
void project_onto_base_plane(std::vector<glm::vec3>& intersections) {
    // Project all points in buffer onto plane
    for (int i = 0; i < intersections.size(); i++) {

        // if the point is above the plane, we wanna decrease its vertical position
        // if the point is below, we wanna increment its position
        bool above = intersections[i].y > s_base.point.y;
        float distance_to_base_plane = glm::abs(intersections[i].y - s_base.point.y);

        if (above)
            intersections[i] -= s_base.norm * distance_to_base_plane;
        else
            intersections[i] += s_base.norm * distance_to_base_plane;
    }
}


// so what this method is trying to do is look at the corners of the camera frustum and 
// convert the screen space coordinates into world space, then from there we can check 
// each edge of the camera frustum in world space against the upper and lower bounding 
// planes of what will become our water volume. If intersections are detected, then we
// can see and so should draw our water volume at those positions, eventually. 
bool checkWorldSpaceIntersection(std::vector<glm::vec3>& intersections, glm::mat4 viewprojection) {

    // STEP TWO:
    // determine if any of the displaceable volume is within the camera frustum
    // if not, don't bother drawing
    bool worldSpaceIntersection = false;

    glm::vec3 frustumCorners[8] = {
      {-1,  1, -1}, // Near Top Left
      { 1,  1, -1}, // Near Top Right
      {-1, -1, -1}, // Near Bottom Left
      { 1, -1, -1}, // Near Bottom Right
      {-1,  1,  1}, // Far Top Left
      { 1,  1,  1}, // Far Top Right
      {-1, -1,  1}, // Far Bottom Left
      { 1, -1,  1}  // Far Bottom Right
    };

    for (int i = 0; i < 8; i++) {
        glm::vec4 corner = glm::inverse(viewprojection) * glm::vec4(frustumCorners[i], 1.0f);
        frustumCorners[i] = glm::vec3(corner) / corner.w;
    }

    int frustumEdges[12][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Near plane edges
        {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Far plane edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
    };




    // Check for intersections between the edges of the camera frustum and the bound planes (Supper and
    // Slower).Store the world - space positions of all intersections in a buffer

    for (int i = 0; i < 12; i++) {
        glm::vec3 contact;
        int a = frustumEdges[i][0];
        int b = frustumEdges[i][1];

        if (lineSegmentPlaneIntersection(contact, frustumCorners[a], frustumCorners[b], s_upper.norm, s_upper.point))
            intersections.push_back(contact);
        if (lineSegmentPlaneIntersection(contact, frustumCorners[a], frustumCorners[b], s_lower.norm, s_lower.point))
            intersections.push_back(contact);
    }


    for (int i = 0; i < 8; i++) {
        if (frustumCorners[i].y < s_upper.point.y && frustumCorners[i].y > s_lower.point.y)
            intersections.push_back(frustumCorners[i]);
    }

    return intersections.size() > 0;

}

bool lineSegmentPlaneIntersection(glm::vec3& contact, glm::vec3 a, glm::vec3 b,
    glm::vec3 normal, glm::vec3 coord) {
    // Normalize the normal vector to ensure consistent magnitude
    normal = glm::normalize(normal);

    // Calculate the plane constant (d) for the equation of the plane: normal.x * x + normal.y * y + normal.z * z = d
    float d = glm::dot(normal, coord);

    // Calculate the ray direction (b - a) and the dot product of the normal with the ray direction
    glm::vec3 ray = b - a;
    float denom = glm::dot(normal, ray);

    // If the denominator is very close to zero, the line is parallel to the plane
    if (glm::abs(denom) < 1e-6f) {
        return false; // No intersection
    }

    // Calculate the parameter t of the intersection point
    float t = (d - glm::dot(normal, a)) / denom;

    // If t is outside the segment (0 <= t <= 1), return false
    if (t < 0.0f || t > 1.0f) {
        return false;
    }

    // The contact point on the line segment
    contact = a + t * ray;

    return true; // There's an intersection within the segment bounds
}



glm::vec3 aimAtHorizon(glm::vec3 camera_front, glm::vec3 projector_position) {
    glm::vec3 dir = glm::normalize(camera_front);

    float t = 30.0f;

    return (dir * t) - projector_position;
    
}


void printVec(glm::vec3 v) {
    std::cout << "x: " << v.x << ", " << "y: " << v.y << ", " << "z: " << v.z << "\n";
}


// loads an image into the char array from input filepath
void load_image(std::vector<unsigned char>& image, std::string filepath) {
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filepath);
    if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    else
        std::cout << "Success loading image from filepath: " << filepath << "\n";
}

// Draws TRIANGLES using input VAO/vertices
void draw(GLuint& VAO, std::vector<glm::vec3> obj_vertices) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size());
    glBindVertexArray(0);
}

// just sets up the window
int setUp() {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Real Time Water Rendering!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    camera.setSpeed(cameraSpeed);

    return 1;
}

// cleans up the window
void cleanUp() {
    glfwTerminate();
}


// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        projector.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        projector.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        projector.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        projector.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
        projector.ProcessKeyboard(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
        projector.ProcessKeyboard(DOWN, deltaTime);
    }
    // Wireframe toggle (on key press, not held)
    bool wireframeKeyCurrentlyPressed = glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS;
    if (wireframeKeyCurrentlyPressed && !wireframeKeyPressedLastFrame)
    {
        wireframe = !wireframe;
        std::cout << "Wireframe mode: " << (wireframe ? "ON" : "OFF") << std::endl;
        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    wireframeKeyPressedLastFrame = wireframeKeyCurrentlyPressed;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// This function may be useful to us, if we can define the vertices of the projected grid
// then this function will give us an easy way to bind them to a VAO/VBO object for drawing
void populate_buffer(GLuint& VAO, GLuint& VBO, const std::vector<glm::vec3>& vertices,
    bool normals, bool tex_coords) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Calculate stride (size of one vertex in bytes)
    int stride = 3; // Position (vec3)
    if (normals) stride += 3; // Normal (vec3)
    if (tex_coords) stride += 3; // Texture Coordinates (vec3)

    stride *= sizeof(float); // Convert to bytes
    int layout_loc = 0;

    // Position Attribute
    glVertexAttribPointer(layout_loc, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    layout_loc++;
    int offset = 3 * sizeof(float); // Offset starts after position data

    if (normals) {
        glVertexAttribPointer(layout_loc, 3, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)(offset));
        glEnableVertexAttribArray(1);
        offset += 3 * sizeof(float); // Move offset past normal data
        layout_loc++;
    }

    if (tex_coords) {
        glVertexAttribPointer(layout_loc, 3, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)(offset));
        glEnableVertexAttribArray(2);
        offset += 3 * sizeof(float); // Move offset past tex
        layout_loc++; // not necessary
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}