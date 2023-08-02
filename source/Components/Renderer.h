#pragma once

#include <map>
#include <utility>
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "GLFW/glfw3.h"
#include "Visual/VisualGrid.h"
#include "Visual/VisualLine.h"
#include "Visual/VisualCube.h"
#include "Visual/VisualPlane.h"
#include "Visual/VisualSphere.h"
#include "Screen.h"


class Renderer
{
private:
    struct Racket
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::vec3 lower_arm_rot =  glm::vec3(0.0f);
        glm::vec3 upper_arm_rot =  glm::vec3(-45.0f, 0.0f, 0.0f);


        Racket() = default;
        Racket(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale) : position(_position), rotation(_rotation), scale(_scale) {}
    };

    std::unique_ptr<Screen> main_screen;
    std::shared_ptr<Camera> main_camera;
    std::unique_ptr<Shader::Material> shadow_mapper_material;

    std::unique_ptr<VisualGrid> main_grid;

    std::unique_ptr<VisualLine> main_x_line;
    std::unique_ptr<VisualLine> main_y_line;
    std::unique_ptr<VisualLine> main_z_line;

    std::shared_ptr<Light> main_light;
    std::unique_ptr<VisualCube> main_light_cube;
    std::unique_ptr<VisualCube> world_cube;
    std::unique_ptr<VisualPlane> texture_cube;

    std::vector<VisualSphere> tennis_balls;

    std::vector<VisualCube> net_cubes;

    std::vector<VisualCube> letter_cubes;

    std::shared_ptr<VisualCube> augusto_racket_cube;
    std::vector<Shader::Material> augusto_racket_materials;

    VisualCube gabrielle_racket_cube;
    VisualCube jack_racket_cube;

    std::vector<Racket> rackets;
    std::vector<Racket> default_rackets;

    int viewport_width, viewport_height;

    bool shadow_mode = true;
    int racket_render_mode = GL_TRIANGLES;
    int selected_player = 4;

    GLuint shadow_map_fbo = 0;
    GLuint shadow_map_depth_tex = 0;

public:
    Renderer(int _initialWidth, int _initialHeight);

    void Init();
    void Render(GLFWwindow *_window, double _deltaTime);

    void DrawOneNet(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);
    void DrawOneAugustoRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);
    void DrawOneGabrielleRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);
    void DrawOneJackRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);

    void DrawOneA(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);
    void DrawOneG(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);
    void DrawOneJ(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride = nullptr);

    void ResizeCallback(GLFWwindow *_window, int _displayWidth, int _displayHeight);
    void InputCallback(GLFWwindow *_window, double _deltaTime);

    GLuint loadTexture(const char *filename);
};
