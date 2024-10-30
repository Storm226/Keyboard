#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Shader.h"
#include <glm/glm.hpp>

class DirectionalLight {
public:
    // Default constructor with default values
    DirectionalLight(Shader& shader,
        const glm::vec3& direction = glm::vec3(5.0f, 7.0f, 5.0f),
        const glm::vec3& ambient = glm::vec3(0.05f),
        const glm::vec3& diffuse = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f));

    // Method to update shader values
    void updateShader();

private:
    Shader& s;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif // DIRECTIONALLIGHT_H
