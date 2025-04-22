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


int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";

    Shader s("Shaders/Plane.vs", "Shaders/basic_object_shader.fs", nullptr, nullptr, nullptr);
    GLuint obj_VAO, obj_VBO;

    glm::mat4 view;
    glm::mat4 perspective;
    glm::mat4 projector_view;

    std::vector<glm::vec3> plane_vertices;
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

    populate_buffer(obj_VAO, obj_VBO, plane_vertices, false, false);
   

 
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

            view = camera.GetViewMatrix();
            perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
            projector.Front = aimAtHorizon(camera.Front, projector.Position);
            projector.updateCameraVectors();
            projector_view = projector.GetViewMatrix();

           
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "camera_view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "camera_projection"), 1, GL_FALSE, glm::value_ptr(perspective));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "projector_view"), 1, GL_FALSE, glm::value_ptr(projector_view));


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