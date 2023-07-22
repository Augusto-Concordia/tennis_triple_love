#pragma once

#include <map>
#include "Camera.h"
#include "Shader.h"
#include "Visual/VisualGrid.h"
#include "GLFW/glfw3.h"
#include "Components/Visual/VisualLine.h"
#include "Components/Visual/VisualCube.h"

class Renderer {
private:
    struct Racket {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    struct Light {
        VisualCube cube;

        glm::vec3 position;
        glm::vec3 color;
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

    std::vector<VisualCube> racket_cubes;
    std::vector<Racket> rackets;
    std::vector<Racket> default_rackets;

    int racket_render_mode = GL_TRIANGLES;
    int selected_player = 4;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Render(GLFWwindow* _window, double _deltaTime);

    void DrawOneNet(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    void DrawOneRacket(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, int player);
    void DrawOneP(glm::mat4 world_transform_matrix);

    void ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow* _window, double _deltaTime);

    void DrawOneI(glm::mat4 world_transform_matrix);

    void DrawOneN(glm::mat4 world_transform_matrix);

    void DrawOneH(glm::mat4 world_transform_matrix);
};

