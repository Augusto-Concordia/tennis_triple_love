#pragma once

#include <map>
#include <utility>
#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"

class Renderer
{
private:
    struct Racket
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        Racket() = default;
        Racket(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale) : position(_position), rotation(_rotation), scale(_scale) {}
    };

    struct Light
    {
        VisualCube cube;

        glm::vec3 position;
        glm::vec3 color;

        Light() = default;
        Light(VisualCube _cube, glm::vec3 _position, glm::vec3 _color) : cube(std::move(_cube)), position(_position), color(_color) {}
    };

    std::shared_ptr<Camera> main_camera;
    std::shared_ptr<Shader> default_shader;

    std::unique_ptr<VisualGrid> main_grid;

    std::unique_ptr<VisualLine> main_x_line;
    std::unique_ptr<VisualLine> main_y_line;
    std::unique_ptr<VisualLine> main_z_line;

    std::unique_ptr<Light> main_light;
    std::unique_ptr<VisualCube> world_cube;

    std::vector<VisualCube> net_cubes;

    std::vector<VisualCube> letter_cubes;

    std::shared_ptr<VisualCube> augusto_racket_cube;
    std::vector<Shader::Descriptor> augusto_racket_materials;

    VisualCube gabrielle_racket_cube;
    VisualCube jack_racket_cube;

    std::vector<Racket> rackets;
    std::vector<Racket> default_rackets;

    int racket_render_mode = GL_TRIANGLES;
    int selected_player = 4;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Render(GLFWwindow *_window, double _deltaTime);

    void DrawOneNet(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);
    void DrawOneAugustoRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);
    void DrawOneJackRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);
    void DrawOneGabrielleRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);

    void DrawOneP(glm::mat4 world_transform_matrix);
    void DrawOneG(glm::mat4 world_transform_matrix);

    void ResizeCallback(GLFWwindow *_window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow *_window, double _deltaTime);

    void DrawOneI(glm::mat4 world_transform_matrix);

    void DrawOneN(glm::mat4 world_transform_matrix);

    void DrawOneH(glm::mat4 world_transform_matrix);
};
