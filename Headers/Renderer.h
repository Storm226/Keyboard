#include <glad/glad.h> 
#include <vector>

class Renderer {
public:
    Renderer(const std::vector<float>& vertices);
    ~Renderer();

    void render(); 
    unsigned int cubeVAO, cubeVBO;
private:
    
    void setupMesh(const std::vector<float>& vertices);
};
