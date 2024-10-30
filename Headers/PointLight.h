#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Shader.h"
#include <glm/glm.hpp>

class PointLight {
public:
    // Default constructor with default values
    PointLight(Shader& shader,
        const glm::vec3& position = glm::vec3(5.0f, 7.0f, 5.0f),
        const glm::vec3& ambient = glm::vec3(0.05f),
        const glm::vec3& diffuse = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f),
        float constant = 1.0f,
        float linear = 0.09f,
        float quadratic = 0.032f);

    // Method to update shader values
    void updateShader();

    // Getter and setter for position
    void setPosition(const glm::vec3& newPosition);
    glm::vec3 getPosition() const;

private:
    Shader& s; 
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

#endif // POINTLIGHT_H