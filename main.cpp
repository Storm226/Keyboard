#include <glad/glad.h>
#include <glfw3.h>

#include "Headers/Shader.h"
#include "Headers/Camera.h"

#include "Headers/lodepng.h"


#include "Headers/definitions.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Globals
GLuint skyboxVAO, skyboxVBO, objectVAO, objectVBO;
GLuint cubemapTexture, skyboxShader, objectShader;
glm::mat4 viewMatrix, projMatrix;
float rotationX = 0.0f, rotationY = 0.0f, reflectStrength = 0.8f;
double lastX = 0.0, lastY = 0.0;
bool mousePressed = false;

// Cube map faces
std::vector<std::string> faces = {
    "cubemap/cubemap_posx.png",
    "cubemap/cubemap_negx.png",
    "cubemap/cubemap_posy.png",
    "cubemap/cubemap_negy.png",
    "cubemap/cubemap_posz.png",
    "cubemap/cubemap_negz.png"
};

GLuint loadCubemapWithRawData(std::vector<std::string> faces) {
    cy::GLTextureCubeMap cubemap;
    cubemap.Initialize();

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.GetID());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (GLuint i = 0; i < faces.size(); ++i) {
        std::vector<unsigned char> image;
        unsigned width, height;
        unsigned error = lodepng::decode(image, width, height, faces[i]);
        if (error) {
            std::cerr << "Failed to load cubemap texture: " << faces[i] << " - " << lodepng_error_text(error) << std::endl;
            exit(1);
        }
        cubemap.SetImage((cy::GLTextureCubeMap::Side)i, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, image.data(), width, height);
    }

    cubemap.BuildMipmaps();
    cubemap.SetSeamless(true);
    return cubemap.GetID();
}

int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";


    // STEP ONE: 
    // define the camera and its matrices, and also 
    // get information needed for projector from camera
    camera.Front = glm::vec3(0.0f, 0.0f, 0.0f);
    projector.Front = glm::vec3(0.0f, 0.0f, 0.0f);

    
    cubemapTexture = loadCubemapWithRawData(faces);
         

        //render loop
        // ----------------------------------
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            std::cout << "Stop 1 \n";



            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);

            // we also are interested in defining our projector, which is kinda basically a camera

            glm::mat4 projector_view = projector.GetViewMatrix();
            glm::mat4 M_Projector = glm::inverse(projector_view * perspective);
            

            // STEP TWO:
            // determine if any of the displaceable volume is within the camera frustum
            // if not, don't bother drawing
                bool worldSpaceIntersection = false;

                float tanFovY = glm::tan(FOV * 0.5f);
                float nearHeight = 2.0f * zNear * tanFovY;
                float nearWidth = nearHeight * ASPECT_RATIO;
                float farHeight = 2.0f * zFar * tanFovY;
                float farWidth = farHeight * ASPECT_RATIO;

                glm::vec3 frustumCorners[8] = {
                    { -nearWidth * 0.5f,  nearHeight * 0.5f, -zNear }, // Near Top Left
                    {  nearWidth * 0.5f,  nearHeight * 0.5f, -zNear }, // Near Top Right
                    { -nearWidth * 0.5f, -nearHeight * 0.5f, -zNear }, // Near Bottom Left
                    {  nearWidth * 0.5f, -nearHeight * 0.5f, -zNear }, // Near Bottom Right
                    { -farWidth * 0.5f,   farHeight * 0.5f, -zFar },  // Far Top Left
                    {  farWidth * 0.5f,   farHeight * 0.5f, -zFar },  // Far Top Right
                    { -farWidth * 0.5f,  -farHeight * 0.5f, -zFar },  // Far Bottom Left
                    {  farWidth * 0.5f,  -farHeight * 0.5f, -zFar }   // Far Bottom Right
                };

                // now we need to get those corners into world space by using the inverse viewProjection Matrix
                glm::mat4 inverse_View = glm::inverse(view);
                for (int i = 0; i < 8; i++) {
                    frustumCorners[i] = glm::vec3(inverse_View * glm::vec4(frustumCorners[i], 1.0f));
                }

                std::cout << "Stop 2 \n";

                // now that we have world space coordinates for the frustum we can actually check 
                // for intersections against our upper and lower bounding planes


                // Check for intersections between the edges of the camera frustum and the bound planes (Supper and
                // Slower).Store the world - space positions of all intersections in a buffer

                std::vector<glm::vec3> intersections;

                for (int i = 1; i < 8; i++) {
                    glm::vec3 contact;
                    if (lineSegmentPlaneIntersection(contact, frustumCorners[i - 1], frustumCorners[i], s_upper.norm, s_upper.point))
                        intersections.push_back(contact);
                    if (lineSegmentPlaneIntersection(contact, frustumCorners[i - 1], frustumCorners[i], s_lower.norm, s_lower.point))
                        intersections.push_back(contact);
                }

                std::cout << "Stop 3 \n";

                for (int i = 0; i < 8; i++) {
                    if (frustumCorners[i].y < s_upper.point.y && frustumCorners[i].y > s_lower.point.y)
                        intersections.push_back(frustumCorners[i]);
                }

                std::cout << "Stop 4 \n";

                // hypothetically, we now know if we can see the water or not
                // if this list is empty, we don't draw
                // if the list is not empty, draw the water
                if (intersections.size() > 0)
                    worldSpaceIntersection = true;
                 

                // Project all points in buffer onto plane
                // check this math i think it might be slightly off but i cant be fucked rn
                for (int i = 0; i < intersections.size(); i++) {

                    // if the point is above the plane, we wanna decrease its vertical position
                    // if the point is below, we wanna increment its position
                    bool above = intersections[i].y > s_base.point.y;
                    float distance_to_base_plane = intersections[i].y - s_base.point.y;
                    
                    if(above)
                        intersections[i] -= s_base.norm * distance_to_base_plane;
                    else
                        intersections[i] += s_base.norm * distance_to_base_plane;
                }

                std::cout << "Stop 5 \n";
                
                // so now we wanna transform the points in the buffer to projector space by using inverse of M_projector
                // the x and y span of the visible volume is now the x/y spans of the points in the buffer

                float minX = 0, maxX = 0 , minY = 0, maxY = 0;

                // we can do this transformation and also compute the minX/maxX minY/maxY
                for (int i = 0; i < intersections.size(); i++) {
                    intersections[i] = glm::vec4(intersections[i], 0) * glm::inverse(M_Projector);

                    // x span
                    if (intersections[i].x < minX)
                        minX = intersections[i].x;
                    else if (intersections[i].x > maxX)
                        maxX = intersections[i].x;

                    // y span
                    if (intersections[i].y < minY)
                        minY = intersections[i].y;
                    else if (intersections[i].y > maxY)
                        maxY = intersections[i].y;
                }

                std::cout << "Stop 6 \n";

                // define the range matrix to augment the m_projector matrix
                glm::mat4 range = glm::mat4(1.0);
                range[0][0] = maxX - minX;
                range[0][3] = minX;
                range[1][1] = maxY - minY;
                range[1][3] = minY;

                std::cout << "Stop 6.2 \n";

                M_Projector = range * M_Projector;


                // holy shit we can finally define the fucking vertices that we are gonna pass to shaders 
                // omg omg omg

                // jk we gotta do more intersection tests then we get the final vertices
                   
                std::vector<glm::vec3> pre_transform_vertices = 
                { glm::vec3(0.0f, 0.0f, -1.0f),       // near left   z = -1
                  glm::vec3(0.0f, 0.0f,  1.0f),       // near left   z = 1

                  glm::vec3(1.0f, 0.0f, -1.0f),       // near right
                  glm::vec3(1.0f, 0.0f,  1.0f),       // near right

                  glm::vec3(1.0f, 1.0f, -1.0f),       // far left
                  glm::vec3(1.0f, 1.0f,  1.0f),       // far left

                  glm::vec3(1.0f, 1.0f, -1.0f),       // far right
                  glm::vec3(1.0f, 1.0f,  1.0f)};      // far right
            
                std::cout << "Stop 6.5 \n";


                // MISTAKE
                // this assume four intersectiosn there may be zero
                // thats why NaN
                std::vector<glm::vec3> final_vertices;
                for (int i = 0; i < 8; i+=2) {
                    glm::vec3 a = M_Projector * glm::vec4(pre_transform_vertices[i], 0);
                    glm::vec3 b = M_Projector * glm::vec4(pre_transform_vertices[i+1], 0);
                    glm::vec3 intersection;
                    if (!lineSegmentPlaneIntersection(intersection, a, b, s_base.norm, s_base.point))
                        std::cout << "Intersection not found at step 2.4.6 \n";


                    final_vertices.push_back(intersection);
                }

                std::cout << "Stop 7 \n";

                std::cout << "Attempting print intersection vertices: \n";
                for (int i = 0; i < final_vertices.size(); i++)
                    printVec(final_vertices[i]);


                //Binding the cubemap


            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    cleanUp();

    return 0;
}


void printVec(glm::vec3 v) {
    std::cout << "x: " << v.x << ", " << "y: " << v.y << ", " << "z: " << v.z << "\n";
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
    glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size()/3);
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


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
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


// some of these tests seem to fail
// further investigation is needed
void line_plane_tests() {
    line_plane_ShouldBeTrue1();
    line_plane_ShouldBeTrue2();
    line_plane_ShouldBeTrue3();
    line_plane_ShouldBeTrue4();
    line_plane_ShouldBeTrue5();

    line_plane_ShouldBeFalse1();
    line_plane_ShouldBeFalse2();
    line_plane_ShouldBeFalse3();
    line_plane_ShouldBeFalse4();
    line_plane_ShouldBeFalse5();

}


void line_plane_ShouldBeTrue1() {
    glm::vec3 a(0.0f, 10.0f, 0.0f);
    glm::vec3 b(0.0f, -20.0f, 0.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(1.0f, 1.0f, 0.0f); // Plane at y = 1
    glm::vec3 norm(0.0f, 1.0f, 0.0f); // Horizontal plane
    glm::vec3 c;

    if (lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void line_plane_ShouldBeFalse1() {
    glm::vec3 a(0.0f, -10.0f, 0.0f);
    glm::vec3 b(0.0f, -20.0f, 0.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(1.0f, 1.0f, 0.0f);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);
    glm::vec3 c;

    if (!lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Plane at z = 5, ray moving in z-direction
void line_plane_ShouldBeTrue2() {
    glm::vec3 a(2.0f, 0.0f, -2.0f);
    glm::vec3 b(2.0f, 0.0f, 10.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(0.0f, 0.0f, 5.0f); // Plane at z = 5
    glm::vec3 norm(0.0f, 0.0f, 1.0f); // Plane normal along z
    glm::vec3 c;

    if (lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Plane at z = -5, but ray is going the opposite direction
void line_plane_ShouldBeFalse2() {
    glm::vec3 a(2.0f, 0.0f, -10.0f);
    glm::vec3 b(2.0f, 0.0f, -6.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(0.0f, 0.0f, -5.0f);
    glm::vec3 norm(0.0f, 0.0f, 1.0f);
    glm::vec3 c;

    if (!lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Plane at x = 3, ray moving in x-direction
void line_plane_ShouldBeTrue3() {
    glm::vec3 a(-5.0f, 2.0f, 0.0f);
    glm::vec3 b(6.0f, 2.0f, 0.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(3.0f, 0.0f, 0.0f); // Plane at x = 3
    glm::vec3 norm(1.0f, 0.0f, 0.0f); // Plane normal along x
    glm::vec3 c;

    if (lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Plane at x = 8, but ray never reaches it
void line_plane_ShouldBeFalse3() {
    glm::vec3 a(-5.0f, 2.0f, 0.0f);
    glm::vec3 b(4.0f, 2.0f, 0.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(8.0f, 0.0f, 0.0f);
    glm::vec3 norm(1.0f, 0.0f, 0.0f);
    glm::vec3 c;

    if (!lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Diagonal plane cutting through the origin
void line_plane_ShouldBeTrue4() {
    glm::vec3 a(0.0f, 5.0f, 5.0f);
    glm::vec3 b(0.0f, -5.0f, -5.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(0.0f, 0.0f, 0.0f); // Plane at y = z
    glm::vec3 norm(0.0f, 1.0f, -1.0f); // Normal pointing along (0,1,-1)
    glm::vec3 c;

    if (lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Parallel case, no intersection
void line_plane_ShouldBeFalse4() {
    glm::vec3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(1.0f, 4.0f, 5.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(0.0f, 1.0f, 0.0f);
    glm::vec3 norm(0.0f, 1.0f, -1.0f);
    glm::vec3 c;

    if (!lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Plane tilted diagonally in x-y
void line_plane_ShouldBeTrue5() {
    glm::vec3 a(0.0f, 10.0f, 5.0f);
    glm::vec3 b(10.0f, 0.0f, 5.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(5.0f, 5.0f, 5.0f); // Plane at x + y = 10
    glm::vec3 norm(1.0f, 1.0f, 0.0f);
    glm::vec3 c;

    if (lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

// Ray goes away from plane, no intersection
void line_plane_ShouldBeFalse5() {
    glm::vec3 a(0.0f, 10.0f, 5.0f);
    glm::vec3 b(-10.0f, 20.0f, 5.0f);
    glm::vec3 ray = b - a;
    glm::vec3 ray_origin = a;
    glm::vec3 p(5.0f, 5.0f, 5.0f);
    glm::vec3 norm(1.0f, 1.0f, 0.0f);
    glm::vec3 c;

    if (!lineSegmentPlaneIntersection(c, a, b, norm, p))
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}
