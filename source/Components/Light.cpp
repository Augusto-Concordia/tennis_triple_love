#include "Light.h"

Light::Light(glm::vec3 _position, glm::vec3 _color, float _ambientStrength, float _specularStrength) {
    position = _position;
    color = _color;
    ambient_strength = _ambientStrength;
    specular_strength = _specularStrength;
}
