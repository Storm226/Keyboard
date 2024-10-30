#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Headers/Shader.h"
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
        float quadratic = 0.032f)
        : s(shader), position(position), ambient(ambient), diffuse(diffuse),
        specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}

    // Method to update shader values
    void apply() {
        s.setVec3("p_light.position", position);
        s.setFloat("p_light.constant", constant);
        s.setFloat("p_light.linear", linear);
        s.setFloat("p_light.quadratic", quadratic);
        s.setVec3("p_light.ambient", ambient);
        s.setVec3("p_light.diffuse", diffuse);
        s.setVec3("p_light.specular", specular);
    }

    // Getter and setter for position (optional)
    void setPosition(const glm::vec3& newPosition) {
        position = newPosition;
    }

    glm::vec3 getPosition() const {
        return position;
    }

private:
    Shader& s; // Reference to the shader program
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
#endif