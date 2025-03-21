#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Headers/Shader.h"
#include "Headers/Camera.h"
#include "Headers/stb_image.h"
#include "Headers/PointLight.h"
#include "Headers/DirectionalLight.h"
#include "Headers/SpotLight.h"
#include "Headers/lodepng.h"

#include "Headers/cyGL.h"
#include "Headers/cyTriMesh.h"
#include "Headers/cyVector.h"
#include "Headers/definitions.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void convert_to_vector(cy::TriMesh& mesh, std::vector<glm::vec3>& vertices, bool normal, bool tex_coords);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void generateTexture(std::string filename, unsigned int& textureName, bool alpha);
int setUp();
void cleanUp();
void populate_buffer(GLuint& VAO, GLuint& VBO, const std::vector<glm::vec3>& vertices,
    bool normals, bool tex_coords);
void draw(GLuint& VAO, std::vector<glm::vec3> obj_vertices);
void setupMVP(Shader&s, SpotLight& p);
void append_plane(std::vector<glm::vec3>& vertices, cy::TriMesh& m);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float aspectRatio = (float) SCR_WIDTH /  (float) SCR_HEIGHT;

GLFWwindow* window;

// camera
Camera camera(glm::vec3(40.0f, 30.0f, 55.0f)); 
float cameraSpeed = 15.0f;


float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Frustum 
float nearPlane = 1.0f;
float farPlane = 100.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";
  

    std::string obj_path = "Resources/teapot.obj";

    // Compile shaders
    // ---------------
    Shader shadowmapping("Shaders/shadowmap.vs", "Shaders/shadowmap.fs");
    Shader depthGen("Shaders/depGen.vs", "Shaders/depGen.fs");

    cy::TriMesh mesh;
    GLuint obj_VAO, obj_VBO, cube_VAO, cube_VBO, plane_VAO, plane_VBO;
    std::vector<glm::vec3> vertices;


    if (mesh.LoadFromFileObj(obj_path.c_str()))
        std::cout << "Model successfully loaded \n";

    convert_to_vector(mesh, vertices, true, true);
    append_plane(vertices, mesh);
    populate_buffer(obj_VAO, obj_VBO, vertices, 1, 1);

    // Render to texture steps
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    GLuint framebuffer;
    GLuint depthMap;
    int shadow_width = SCR_WIDTH, shadow_height = SCR_HEIGHT;

    // Generate the frame buffer and depth map
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // allocates space on gpu for texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // note this is shadowmapping step
    //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, NULL);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    SpotLight p(shadowmapping);
    p.setPosition(glm::vec3(0.0f, 30.0f, 40.0f));
    p.setDirection(glm::vec3(0.0f));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "issue with framebuffer" << std::endl;


        //render loop
        // ----------------------------------
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);
       
            // Don't forget to use the shader program
            shadowmapping.use();
            //SpotLight p(blinn, glm::vec3(0.0f, 30.0f, 40.0f), glm::vec3(0.0f), COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, 3.0f, 3.0f, 0.2f);
           

            // this is setting up mvp for final render pass
            setupMVP(shadowmapping, p);

            // we also need setup lightspace transformations, setting up view volume from lights perspective
            glm::mat4 lightViewMatrix = glm::lookAt(p.position, glm::vec3(0.0f), camera.Up);
            glm::mat4 lightProjectionMatrix = glm::perspective(2.0f * p.outerCutOff, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            glm::mat4 MLP = lightProjectionMatrix * lightViewMatrix;
            glm::mat4 mShadow = MLP;
            glm::mat4 T = glm::translate(glm::mat4(1.0), glm::vec3(0.5f, 0.5f, 0.5f));
            glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
            mShadow = T * S * mShadow;


            // render the depth map
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
                glViewport(0, 0, shadow_width, shadow_height);
                glClear(GL_DEPTH_BUFFER_BIT);
                depthGen.use();
                glUniformMatrix4fv(glGetUniformLocation(depthGen.ID, "mvp"), 1, GL_FALSE, glm::value_ptr(MLP));
                draw(obj_VAO, vertices);
                

            // render the camera view
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFBO);
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                shadowmapping.use();
                glUniform1i(glGetUniformLocation(shadowmapping.ID, "depthMap"), 1);
                glUniformMatrix4fv(glGetUniformLocation(shadowmapping.ID, "matrixShadow"), 1, GL_FALSE, glm::value_ptr(mShadow));

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthMap);

                draw(obj_VAO, vertices);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    cleanUp();

    return 0;
}



void setupMVP(Shader& s, SpotLight& p) {
    glm::mat4 mvp = glm::mat4(0.5f);
    glm::mat4 model = glm::mat4(0.5f);
    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);


    glm::vec3 k_d = COLOR_ORANGE;
    glm::vec3 k_s = COLOR_WHITE;

    glUniform3f(glad_glGetUniformLocation(s.ID, "k_d"), k_d.x, k_d.y, k_d.z);
    glUniform3f(glad_glGetUniformLocation(s.ID, "k_s"), k_s.x, k_s.y, k_s.z);
    glUniform3f(glad_glGetUniformLocation(s.ID, "light_clr"), p.getDiffuseColor().x, p.getDiffuseColor().y, p.getDiffuseColor().z);
    glUniform3f(glad_glGetUniformLocation(s.ID, "light_pos"), p.getPosition().x, p.getPosition().y, p.getPosition().z);
    glUniform3f(glad_glGetUniformLocation(s.ID, "view_dir"), camera.Position.x, camera.Position.y, camera.Position.z);


    glUniformMatrix4fv(glGetUniformLocation(s.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(s.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(s.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void draw(GLuint& VAO, std::vector<glm::vec3> obj_vertices) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size());
    glBindVertexArray(0);
}

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "KeyBoard", NULL, NULL);
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

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    camera.setSpeed(cameraSpeed);
    stbi_set_flip_vertically_on_load(false);

    return 1;
}

void cleanUp() {
    glfwTerminate();
}
void convert_to_vector(cy::TriMesh& mesh, std::vector<glm::vec3>& vertices, bool normal, bool tex_coords) {
    for (int i = 0; i < mesh.NF(); i++) { // Loop over faces
        cy::TriMesh::TriFace face = mesh.F(i); // Vertex indices

        // Get normal and texture coordinate indices (only if needed)
        cy::TriMesh::TriFace normalFace, texFace;
        if (normal) normalFace = mesh.FN(i);
        if (tex_coords) texFace = mesh.FT(i);

        for (int j = 0; j < 3; j++) { // Loop over triangle vertices
            cy::Vec3f v = mesh.V(face.v[j]); // Vertex position
            vertices.push_back(glm::vec3(v.x, v.y, v.z));

            if (normal) {
                cy::Vec3f n = mesh.VN(normalFace.v[j]); // Normal
                vertices.push_back(glm::vec3(n.x, n.y, n.z));
            }

            if (tex_coords) {
                cy::Vec3f t = mesh.VT(texFace.v[j]); // Texture coordinates (3D)
                vertices.push_back(glm::vec3(t.x, t.y, t.z));
            }
        }
    }
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
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


// Given a filename, a texture name, and t/f flag for alpha, 
// initializes tecture name = texture(filename);
// -------------------------------------------------
void generateTexture(std::string filename, unsigned int& textureName, bool alpha) {
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load((filename).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        if (!alpha) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}


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

    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    int offset = 3 * sizeof(float); // Offset starts after position data

    if (normals) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)(offset));
        glEnableVertexAttribArray(1);
        offset += 3 * sizeof(float); // Move offset past normal data
    }

    if (tex_coords) {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(intptr_t)(offset));
        glEnableVertexAttribArray(2);
        offset += 3 * sizeof(float); // Move offset past tex
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void append_plane(std::vector<glm::vec3>& vertices, cy::TriMesh& m) {
    if (!m.IsBoundBoxReady())
        m.ComputeBoundingBox();

    cy::Vec3f min = m.GetBoundMin();
    cy::Vec3f max = m.GetBoundMax();

    glm::vec3 nearLeft = glm::vec3(min.x, min.y, max.z) * 2.5f; // Near (closer to camera)
    glm::vec3 nearRight = glm::vec3(max.x, min.y, max.z) * 2.5f;
    glm::vec3 farLeft = glm::vec3(min.x, min.y, min.z) * 2.5f; // Far (away from camera)
    glm::vec3 farRight = glm::vec3(max.x, min.y, min.z) * 2.5f;

    glm::vec3 nrm = glm::vec3(0.0f, 1.0f, 0.0f); 
    glm::vec3 texBL = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 texBR = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 texTR = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 texTL = glm::vec3(0.0f, 1.0f, 0.0f);

    // First Triangle: bottomLeft → bottomRight → topLeft
    vertices.push_back(nearLeft); vertices.push_back(nrm); vertices.push_back(texBL);
    vertices.push_back(nearRight); vertices.push_back(nrm); vertices.push_back(texBR);
    vertices.push_back(farLeft); vertices.push_back(nrm); vertices.push_back(texTL);

    // Second Triangle: bottomRight → topRight → topLeft
    vertices.push_back(nearRight); vertices.push_back(nrm); vertices.push_back(texBR);
    vertices.push_back(farRight); vertices.push_back(nrm); vertices.push_back(texTR);
    vertices.push_back(farLeft); vertices.push_back(nrm); vertices.push_back(texTL);
}
