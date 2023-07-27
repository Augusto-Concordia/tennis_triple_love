#pragma once

#include "glm/vec3.hpp"

struct Light {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);

    float ambient_strength = 0.1f;
    float specular_strength = 0.5f;

    Light() = default;
    Light(glm::vec3 _position, glm::vec3 _color, float _ambientStrength, float _specularStrength);
};
