#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Headers/Shader.h"
#include "Headers/Camera.h"
#include "Headers/stb_image.h"
#include "Headers/PointLight.h"
#include "Headers/DirectionalLight.h"
#include "Headers/SpotLight.h"
#include "Headers/lodepng.h"

#include "Headers/cyTriMesh.h"
#include "Headers/cyVector.h"
#include "Headers/definitions.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define PRINT std::cout<<

bool geometryPath = false;
bool tessPath = 1;


int main(int argc, char** argv)
{
    if (!setUp())
        std::cout << "FAILURE DURING SETUP\n";
  
    load_image(normal_map, nrm_map_path);
    load_image(disp_map, height_map_path);

    GLuint normal_map_tex, disp_map_tex;
    
    glGenTextures(1, &normal_map_tex);
    glBindTexture(GL_TEXTURE_2D, normal_map_tex);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, normal_map.data());

    glGenTextures(1, &disp_map_tex);
    glBindTexture(GL_TEXTURE_2D, disp_map_tex);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, disp_map.data());



    // Compile shaders
    // ---------------
    Shader normal_mapping("Shaders/normal_mapping.vs", "Shaders/normal_mapping.fs");
    Shader geo("Shaders/geo.vs", "Shaders/geo.fs", "Shaders/geo.gs");
    Shader tess("Shaders/tess.vs", "Shaders/tess.fs", nullptr, "Shaders/tess_control.txt", "Shaders/tess_eval.txt");

    normal_mapping.use();
    SpotLight p(normal_mapping);
    p.setPosition(glm::vec3(0.0f, 10.0f, -10.0f));
    GLuint obj_VAO, obj_VBO;
    std::vector<glm::vec3> vertices;

    append_plane(vertices);
    populate_buffer(obj_VAO, obj_VBO, vertices, true, true);

        //render loop
        // ----------------------------------
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);
       
            normal_mapping.use();
            setupMVP(normal_mapping, p, 1);

            glBindTexture(GL_TEXTURE_2D, normal_map_tex);
            draw(obj_VAO, vertices);
           
            // if we have a tesselation path provided, we need to not just draw the plane like in normal mapping but also tesselate it
            if (tessPath) {
                tess.use();
                glBindTexture(GL_TEXTURE_2D, disp_map_tex);

                //Here we are specifying that each set of four verticies refer to a single patch
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                setupMVP(tess, p, 2);

                draw(obj_VAO, vertices);
                
            }
            else if (geometryPath) {
                geo.use();
                setupMVP(geo, p, true);

                draw(obj_VAO, vertices);
            }


            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    cleanUp();

    return 0;
}

/// <summary>
/// Initializes input image vector from filepath 
/// </summary>
/// <param name="image"></param>
/// <param name="filepath"></param>
void load_image(std::vector<unsigned char>& image, std::string filepath) {
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filepath);
    if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    else
        std::cout << "Success loading image from filepath: " << filepath << "\n";
}


// 1 for geometry shader
// 2 for tesselation
void setupMVP(Shader& s, SpotLight& p, int type) {
    glm::mat4 model = glm::mat4(100.0f);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1, 0, 0));
    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);


    glUniformMatrix4fv(glGetUniformLocation(s.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(s.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(s.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec3 k_d = COLOR_ORANGE;
    glm::vec3 k_s = COLOR_WHITE;

    if (type == 1) {
        glUniform3f(glGetUniformLocation(s.ID, "k_d"), k_d.x, k_d.y, k_d.z);
        glUniform3f(glGetUniformLocation(s.ID, "k_s"), k_s.x, k_s.y, k_s.z);
        glUniform3f(glGetUniformLocation(s.ID, "light_pos"), p.getPosition().x, p.getPosition().y, p.getPosition().z);
        glUniform3f(glGetUniformLocation(s.ID, "view_dir"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(s.ID, "light_clr"), p.getDiffuseColor().x, p.getDiffuseColor().y, p.getDiffuseColor().z);
    }
    else if (type == 2) {
        /*glUniform3f(glGetUniformLocation(s.ID, "k_d"), k_d.x, k_d.y, k_d.z);
        glUniform3f(glGetUniformLocation(s.ID, "k_s"), k_s.x, k_s.y, k_s.z);
        glUniform3f(glGetUniformLocation(s.ID, "light_pos"), p.getPosition().x, p.getPosition().y, p.getPosition().z);
        glUniform3f(glGetUniformLocation(s.ID, "view_dir"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(s.ID, "light_clr"), p.getDiffuseColor().x, p.getDiffuseColor().y, p.getDiffuseColor().z);*/
    }
}

void draw(GLuint& VAO, std::vector<glm::vec3> obj_vertices) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size()/3);
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
    // PRINT "Hello\n";
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        geometryPath = !geometryPath;
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


void append_plane(std::vector<glm::vec3>& vertices) {

    cy::Vec3f min = { -5.0f, -5.0f, 0.0f };
    cy::Vec3f max = {  5.0f,  5.0f, 0.0f };

    glm::vec3 nearLeft = glm::vec3(min.x, min.y, max.z);
    glm::vec3 nearRight = glm::vec3(max.x, min.y, max.z);
    glm::vec3 farLeft = glm::vec3(min.x, max.y, min.z);
    glm::vec3 farRight = glm::vec3(max.x, max.y, min.z);

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
