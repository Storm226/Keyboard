#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "Headers/Shader.h"
#include "Headers/Camera.h"
#include "Headers/stb_image.h"
#include "Headers/Shapes.h"
#include "Headers/PointLight.h"
#include "Headers/DirectionalLight.h"
#include "Headers/SpotLight.h"
#include "Headers/Renderer.h"
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void generateTexture(std::string filename, unsigned int& textureName, bool alpha);
void setupLightCube(Shader& light_cube_shader, glm::vec3& p_light_position, glm::mat4& projection);
int setUp();
void cleanUp();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
GLFWwindow* window;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f)); 

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Frustum stuff
float nearPlane = 1.0f;
float farPlane = 1000.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";
    camera.setSpeed(5.0f);

    // Compile shaders
    // ---------------
    Shader base_shader("Shaders/BlinnPhong.vs", "Shaders/BlinnPhong.fs");
    Shader light_cube_shader("Shaders/lightCube.vs", "Shaders/lightCube.fs");

    // get geometry, normals, and tex coords
    std::vector<float> vertices = Shapes::getCube();
    std::unique_ptr<Renderer> r{ new Renderer(vertices)};
    
    unsigned int diffuse_map, specular_map;
    generateTexture("Resources/container2.png", diffuse_map, true);
    generateTexture("Resources/container2_specular.png", specular_map, true);
    base_shader.use();
    base_shader.setInt("material.diffuse", 0);
    base_shader.setInt("material.specular", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Don't forget to use the shader program
        base_shader.use();

        // projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);

        base_shader.setVec3("view_position", camera.Position);
        base_shader.setFloat("material.shininess", 32.0f);

        // directional light
        std::unique_ptr<DirectionalLight> d{ new DirectionalLight(base_shader)};
        d->updateShader();

        float rotation_speed = .5f;
        float angle = rotation_speed * glfwGetTime();
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        // point light
        glm::vec3 p_light_position = rotation * glm::vec4(glm::vec3(5.0f, 7.0f, 5.0f), 1.0f);
     
        std::unique_ptr<PointLight> p{ new PointLight(base_shader) };
        p->setPosition(p_light_position);
        p->updateShader();

        // spotLight
        std::unique_ptr<SpotLight> sl{ new SpotLight(base_shader) };
        sl->setDirection(camera.Front);
        sl->setPosition(camera.Position);
        sl->updateShader();
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        // MVP Transformations
        glm::mat4 model = glm::mat4(1.0f);
        base_shader.setMat4("model", model);
        base_shader.setMat4("view", camera.GetViewMatrix());
        base_shader.setMat4("projection", projection);

        // Draw box
        r->render();

        // Draw lights
        setupLightCube(light_cube_shader, p_light_position, projection);
        r->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUp();

    return 0;
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

    return 1;
}

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

void setupLightCube(Shader& light_cube_shader, glm::vec3& p_light_position, glm::mat4& projection) {
    glm::mat4 model_lighting = glm::mat4(1.0f);
    model_lighting = glm::scale(model_lighting, glm::vec3(.5f, .5f, .5f));
    model_lighting = glm::translate(model_lighting, p_light_position);

    light_cube_shader.use();
    light_cube_shader.setMat4("model", model_lighting);
    light_cube_shader.setMat4("view", camera.GetViewMatrix());
    light_cube_shader.setMat4("projection", projection);
}