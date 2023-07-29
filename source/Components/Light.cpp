#include "Light.h"
#include "Utility/Transform.hpp"

Light::Light(glm::vec3 _position, glm::vec3 _color, float _ambientStrength, float _specularStrength) {
    position = _position;
    color = _color;
    ambient_strength = _ambientStrength;
    specular_strength = _specularStrength;

    UpdateView();
    UpdateProjection();
}

void Light::SetPosition(const glm::vec3 &_position) {
    position = _position;

    //SetTarget(position + light_forward * target_distance);

    UpdateView();
}

void Light::SetTarget(const glm::vec3 &_target) {
    position = _target;

    UpdateView();
}

glm::vec3 Light::GetPosition() const {
    return position;
}

glm::vec3 Light::GetColor() const {
    return color;
}

glm::mat4 Light::GetViewProjection() const {
    return projection_matrix * view_matrix;
}

void Light::UpdateView() {
    light_forward = glm::normalize(position - target);
    light_right = glm::normalize(glm::cross(light_forward, Transforms::UP));
    light_up = glm::normalize(glm::cross(light_right, light_forward));

    view_matrix = glm::lookAt(position, target, light_up);
}

void Light::UpdateProjection() {
    projection_matrix = glm::perspective(glm::radians(Light::FOV), 2048.0f / 2048.0f, Light::NEAR_PLANE, Light::FAR_PLANE);
}
