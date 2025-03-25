#include "../Headers/SpotLight.h"

// Constructor definition
SpotLight::SpotLight(Shader& shader,
    const glm::vec3& position,
    const glm::vec3& direction,
    float cutOff,
    float outerCutOff,
    float constant,
    float linear,
    float quadratic,
    const glm::vec3& ambient,
    const glm::vec3& diffuse,
    const glm::vec3& specular)
    : s(shader), position(position), direction(direction), cutOff(cutOff),
    outerCutOff(outerCutOff), constant(constant), linear(linear),
    quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) {}

// Method to update shader values
void SpotLight::updateShader() {
    s.setVec3("s_light.position", position);
    s.setVec3("s_light.direction",direction);
    s.setFloat("s_light.cutoff", cutOff);
    s.setFloat("s_light.outer_cutoff", outerCutOff);
    s.setFloat("s_light.constant", constant);
    s.setFloat("s_light.linear", linear);
    s.setFloat("s_light.quadratic", quadratic);
    s.setVec3("s_light.ambient", ambient);
    s.setVec3("s_light.diffuse", diffuse);
    s.setVec3("s_light.specular", specular);
}

// Setter for position
void SpotLight::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

// Setter for direction
void SpotLight::setDirection(const glm::vec3& newDirection) {
    direction = newDirection;
}

const glm::vec3 SpotLight::getDiffuseColor() {
    return diffuse;
}

const glm::vec3 SpotLight::getPosition() {
    return position;
}
void SpotLight::setColor(glm::vec3 clr) {
    diffuse = clr;
    ambient = clr;
    specular = clr;
}
