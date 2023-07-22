#pragma once

#include <memory>
#include <vector>
#include "glm/vec3.hpp"
#include "Components/Shader.h"

class VisualObject {
public:
    // Transform properties
    glm::vec3 position, rotation, scale;

    // Descriptor for the shader used by this object
    Shader::Descriptor shader_descriptor;

protected:
    // OpenGL shader
    std::shared_ptr<Shader> shader;

    // Vertices and indices used by this object
    // May or may not be used, depending on the implementation
    std::vector<float> vertices;
    std::vector<int> indices;

    // OpenGL buffers
    GLuint vertex_array_o;
    GLuint vertex_buffer_o;
    GLuint element_buffer_o;

public:
    explicit VisualObject(glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _rotation = glm::vec3(0.0f), glm::vec3 _scale = glm::vec3(1.0f), Shader::Descriptor _descriptor = Shader::Descriptor());

    virtual void Draw(const glm::mat4& _viewProjection, const glm::vec3 &_cameraPosition, int render_mode) = 0;
    virtual void DrawFromMatrix(const glm::mat4& _viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4& _transformMatrix, int _renderMode) = 0;

protected:
    virtual void SetupGlBuffersVerticesOnly();
    virtual void SetupGlBuffersVerticesAndNormalsOnlyNoIndices();
};
