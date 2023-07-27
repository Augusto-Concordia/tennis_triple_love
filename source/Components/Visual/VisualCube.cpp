#include "VisualCube.h"
#include "Utility/Transform.hpp"

VisualCube::VisualCube(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale, glm::vec3 _transformOffset, Shader::Material _material) : VisualObject(_position, _rotation, _scale, _material)
{
    // vertices with their normals
    vertices = {
        // top face, top triangle
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,

        // top face, bottom triangle
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        0.0f,
        1.0f,

        // right side, top triangle
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,

        // right side, bottom triangle
        0.5f,
        0.5f,
        0.5f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        1.0f,
        0.0f,
        0.0f,

        // front side, top triangle
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,

        // front side, bottom triangle
        -0.5f,
        0.5f,
        0.5f,
        0.0f,
        1.0f,
        0.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,
        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        1.0f,
        0.0f,

        // left side, top triangle
        -0.5f,
        0.5f,
        0.5f,
        -1.0f,
        0.0f,
        0.0f,
        -0.5f,
        -0.5f,
        0.5f,
        -1.0f,
        0.0f,
        0.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        -1.0f,
        0.0f,
        0.0f,

        // left side, bottom triangle
        -0.5f,
        0.5f,
        0.5f,
        -1.0f,
        0.0f,
        0.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        -1.0f,
        0.0f,
        0.0f,
        -0.5f,
        0.5f,
        -0.5f,
        -1.0f,
        0.0f,
        0.0f,

        // back side, top triangle
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        -1.0f,
        0.0f,
        0.5f,
        -0.5f,
        0.5f,
        0.0f,
        -1.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        -1.0f,
        0.0f,

        // back side, bottom triangle
        -0.5f,
        -0.5f,
        0.5f,
        0.0f,
        -1.0f,
        0.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        -1.0f,
        0.0f,
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        -1.0f,
        0.0f,

        // bottom face, top triangle
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,
        0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,

        // bottom face, bottom triangle
        -0.5f,
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,
        0.5f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,
        -0.5f,
        0.5f,
        -0.5f,
        0.0f,
        0.0f,
        -1.0f,
    };

    for (int i = 0; i < vertices.size(); i += 6)
    {
        vertices[i] += _transformOffset.x;
        vertices[i + 1] += _transformOffset.y;
        vertices[i + 2] += _transformOffset.z;
    }

    VisualObject::SetupGlBuffersVerticesAndNormalsOnlyNoIndices();
}

void VisualCube::Draw(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, int _renderMode, const Shader::Material *material)
{
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = Transforms::RotateDegrees(model_matrix, rotation);
    model_matrix = glm::scale(model_matrix, scale);

    DrawFromMatrix(_viewProjection, _cameraPosition, model_matrix, _renderMode, material);
}

void VisualCube::DrawFromMatrix(const glm::mat4 &_viewProjection, const glm::vec3 &_cameraPosition, const glm::mat4 &_transformMatrix, int _renderMode, const Shader::Material *_material)
{
    // bind the vertex array to draw
    glBindVertexArray(vertex_array_o);

    const Shader::Material *current_material = &material;

    // set the material to use on this frame
    if (_material != nullptr)
        current_material = _material;

    shader->Use();
    shader->SetModelMatrix(_transformMatrix);
    shader->SetViewProjectionMatrix(_viewProjection);

    shader->SetVec3("u_color", current_material->color);
    shader->SetFloat("u_alpha", current_material->alpha);

    shader->SetVec3("u_cam_pos", _cameraPosition);

    shader->SetVec3("u_light_pos", current_material->light_position);
    shader->SetVec3("u_light_color", current_material->light_color);

    shader->SetFloat("u_ambient_strength", current_material->ambient_strength);
    shader->SetFloat("u_specular_strength", current_material->specular_strength);
    shader->SetInt("u_shininess", current_material->shininess);

    glLineWidth(current_material->line_thickness);
    glPointSize(current_material->point_size);

    // draw vertices according to their indices
    glDrawArrays(_renderMode, 0, vertices.size());
}
