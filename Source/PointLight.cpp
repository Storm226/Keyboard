#include "../Headers/PointLight.h"

// Constructor definition
PointLight::PointLight(Shader& s,
    const glm::vec3& position,
    const glm::vec3& ambient,
    const glm::vec3& diffuse,
    const glm::vec3& specular,
    float constant,
    float linear,
    float quadratic)
    : s(s), position(position), ambient(ambient), diffuse(diffuse),
    specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}

// Method to update shader values
void PointLight::updateShader() {
    s.setVec3("p_light.position", position);
    s.setFloat("p_light.constant", constant);
    s.setFloat("p_light.linear", linear);
    s.setFloat("p_light.quadratic", quadratic);
    s.setVec3("p_light.ambient", ambient);
    s.setVec3("p_light.diffuse", diffuse);
    s.setVec3("p_light.specular", specular);
}

// Setter for position
void PointLight::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

// Getter for position
glm::vec3 PointLight::getPosition() const {
    return position;
}

glm::vec3 PointLight::getDiffuseColor() const {
    return diffuse;
}
