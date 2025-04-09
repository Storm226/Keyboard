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


void printVec(glm::vec3 v);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
int setUp();
void cleanUp();
void populate_buffer(GLuint& VAO, GLuint& VBO, const std::vector<glm::vec3>& vertices,
    bool normals, bool tex_coords);
void draw(GLuint& VAO, std::vector<glm::vec3> obj_vertices);
void load_image(std::vector<unsigned char>& image, std::string filepath);
bool lineSegmentPlaneIntersection(glm::vec3& contact, glm::vec3 ray, glm::vec3 rayOrigin,
    glm::vec3 normal, glm::vec3 coord);


// TESTS
void line_plane_tests();
void line_plane_ShouldBeTrue1();
void line_plane_ShouldBeTrue2();
void line_plane_ShouldBeTrue3();
void line_plane_ShouldBeTrue4();
void line_plane_ShouldBeTrue5();

void line_plane_ShouldBeFalse1();
void line_plane_ShouldBeFalse2();
void line_plane_ShouldBeFalse3();
void line_plane_ShouldBeFalse4();
void line_plane_ShouldBeFalse5();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float ASPECT_RATIO = (float)SCR_WIDTH / (float)SCR_HEIGHT;

// the window 
GLFWwindow* window;

// camera
float cameraSpeed = 15.0f;
float zNear = 1.0f;
float zFar = 100.0f;

Camera camera(glm::vec3(0.0f, 20.0f, 0.0f));
Camera projector(glm::vec3(0.0f, 20.0f, 0.0f));

float FOV = glm::radians(45.0f);

// input stuff
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Water rendering

struct plane {
    glm::vec3 point;
    glm::vec3 norm;
};

// these are the world space positions of our upper and lower planes which 
// together encaspulate the volume of displaced points
plane s_upper = { glm::vec3(0.0f,  10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
plane s_base = { glm::vec3(0.0f,    0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
plane s_lower = { glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };



