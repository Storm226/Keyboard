#define WINPOS 100,100
#define WINSIZE WIDTH,HEIGHT
#define WIDTH width
#define HEIGHT height
#define DISPLAYMODE GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH
#define DEBUG 0

#define COLOR_BLACK       glm::vec3(0.0f, 0.0f, 0.0f)
#define COLOR_WHITE       glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_RED         glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN       glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_BLUE        glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_YELLOW      glm::vec3(1.0f, 1.0f, 0.0f)
#define COLOR_CYAN        glm::vec3(0.0f, 1.0f, 1.0f)
#define COLOR_MAGENTA     glm::vec3(1.0f, 0.0f, 1.0f)
#define COLOR_ORANGE      glm::vec3(1.0f, 0.5f, 0.0f)
#define COLOR_PURPLE      glm::vec3(0.5f, 0.0f, 0.5f)
#define COLOR_PINK        glm::vec3(1.0f, 0.4f, 0.7f)
#define COLOR_BROWN       glm::vec3(0.6f, 0.3f, 0.2f)
#define COLOR_GRAY        glm::vec3(0.5f, 0.5f, 0.5f)
#define COLOR_DARK_GRAY   glm::vec3(0.3f, 0.3f, 0.3f)
#define COLOR_LIGHT_GRAY  glm::vec3(0.7f, 0.7f, 0.7f)
#define COLOR_GOLD        glm::vec3(1.0f, 0.84f, 0.0f)
#define COLOR_SILVER      glm::vec3(0.75f, 0.75f, 0.75f)
#define COLOR_LIME        glm::vec3(0.5f, 1.0f, 0.0f)
#define COLOR_TEAL        glm::vec3(0.0f, 0.5f, 0.5f)
#define COLOR_INDIGO      glm::vec3(0.29f, 0.0f, 0.51f)

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
void setupMVP(Shader& s, SpotLight& p, int type);
void append_plane(std::vector<glm::vec3>& vertices);
void load_image(std::vector<unsigned char>& image, std::string filepath);

std::string nrm_map_path("C:\\Users\\18045\\Programming\\src\\Keyboard\\Resources\\teapot_normal.png");
std::vector<unsigned char> normal_map;

std::string height_map_path("C:\\Users\\18045\\Programming\\src\\Keyboard\\Resources\\teapot_disp.png");
std::vector<unsigned char> disp_map;

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

GLFWwindow* window;


// camera
Camera camera(glm::vec3(8.0f, 2.0f, 8.0f));
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

