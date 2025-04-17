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


Camera camera(glm::vec3(0.0f,5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -84.0f, 0.0f); 

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};


GLuint cubemapTexture;
// Cube map faces
//std::vector<std::string> faces = {
//    "cubemap/cubemap_posx.png",
//    "cubemap/cubemap_negx.png",
//    "cubemap/cubemap_posy.png",
//    "cubemap/cubemap_negy.png",
//    "cubemap/cubemap_posz.png",
//    "cubemap/cubemap_negz.png"
//};

std::vector<std::string> faces = {
    "miramar_rt.png",
    "miramar_lf.png",
    "miramar_up.png",
    "miramar_dn.png",
    "miramar_ft.png",
    "miramar_bk.png"
};

GLuint loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	unsigned width, height;
	std::vector<unsigned char> image;

	for (GLuint i = 0; i < faces.size(); i++) {
		image.clear();
		if (lodepng::decode(image, width, height, faces[i]) == 0) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
				width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		}
		else {
			std::cout << "Failed to load cubemap texture: " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";

    Shader s("Shaders/Plane.vs", "Shaders/basic_object_shader.fs", nullptr,  "Shaders/tess_control.txt", "Shaders/tess_eval.txt");
    Shader skybox("skybox.vs", "skybox.fs", nullptr, nullptr, nullptr);
    GLuint obj_VAO, obj_VBO;

    glm::mat4 view;
    glm::mat4 perspective;
    glm::mat4 camera_viewprojection;

   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    s.use();
    cubemapTexture = loadCubemap(faces);
    glUseProgram(s.ID);
    glUniform1i(glGetUniformLocation(s.ID, "skybox"), 0);    

    std::vector<glm::vec3> patchVerts = {
        {-1.0f, 0.0f, -1.0f},
        { 1.0f, 0.0f, -1.0f},
        { 1.0f, 0.0f,  1.0f},
        {-1.0f, 0.0f,  1.0f}
    };

    populate_buffer(obj_VAO, obj_VBO, patchVerts, false, false);
        //render loop
        // ----------------------------------
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            view = camera.GetViewMatrix();
            perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
           
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
            glm::mat4 model = glm::mat4(1.0f); 
            glUniformMatrix4fv(glGetUniformLocation(s.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glUniform3f(glGetUniformLocation(s.ID, "cameraWorldPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniform3f(glGetUniformLocation(s.ID, "lightPosWorld"), 15.0f, 30.0f, -15.0f);
            glUniform3f(glGetUniformLocation(s.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
            glUniform3f(glGetUniformLocation(s.ID, "objectColor"), 0.0f, 0.0f, 1.0f);

            float time = glfwGetTime();
            glUniform1f(glGetUniformLocation(s.ID, "time"), time);

            //glUniformMatrix4fv(glGetUniformLocation(skybox.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
            //glUniformMatrix4fv(glGetUniformLocation(skybox.ID, "projection"), 1, GL_FALSE, glm::value_ptr(perspective));
            //glUniformMatrix4fv(glGetUniformLocation(skybox.ID, "skybox"), 1, GL_FALSE, glm::value_ptr(perspective));


            s.use();
            glPatchParameteri(GL_PATCH_VERTICES, 4); 
            glBindVertexArray(obj_VAO);
            glPatchParameteri(GL_PATCH_VERTICES, 4); 
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_PATCHES, 0, 4);

            glDepthFunc(GL_LEQUAL);
            skybox.use();
            view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            skybox.setMat4("view", view);
            skybox.setMat4("projection", perspective);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDepthMask(GL_FALSE);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);

            std::cout << camera.Pitch << "\n";

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    cleanUp();

    return 0;
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
    glDrawArrays(GL_POINTS, 0, obj_vertices.size());
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