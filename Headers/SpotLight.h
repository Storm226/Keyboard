#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Shader.h"
#include <glm/glm.hpp>

class SpotLight {
public:
    // Constructor with default values
    SpotLight(Shader& shader,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f),
        float cutOff = glm::cos(glm::radians(12.5f)),
        float outerCutOff = glm::cos(glm::radians(20.0f)),
        float constant = 1.0f,
        float linear = 0.09f,
        float quadratic = 0.032f,
        const glm::vec3& ambient = glm::vec3(0.0f),
        const glm::vec3& diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
        const glm::vec3& specular = glm::vec3(0.7f, 0.7f, 0.7f));

    // Method to update shader values
    void updateShader();

    // Setters for position and direction, if needed
    void setPosition(const glm::vec3& newPosition);
    void setDirection(const glm::vec3& newDirection);
    const glm::vec3 getPosition();
    const glm::vec3 getDiffuseColor();

    glm::vec3 position;
    glm::vec3 direction;
    float outerCutOff;

private:
    Shader& s;
    
    float cutOff;
    
    float constant;
    float linear;
    float quadratic;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif // SPOTLIGHT_H
