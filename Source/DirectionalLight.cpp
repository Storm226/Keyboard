#include "../Headers/DirectionalLight.h"

// Constructor definition
DirectionalLight::DirectionalLight(Shader& shader,
    const glm::vec3& direction,
    const glm::vec3& ambient,
    const glm::vec3& diffuse,
    const glm::vec3& specular)
    : s(shader), direction(direction), ambient(ambient), diffuse(diffuse),
    specular(specular) {}

// Method to update shader values
void DirectionalLight::updateShader() {
    s.setVec3("d_light.direction", direction);
    s.setVec3("d_light.ambient", ambient);
    s.setVec3("d_light.diffuse", diffuse);
    s.setVec3("d_light.specular", specular);
}
