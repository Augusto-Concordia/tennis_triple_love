#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Transform.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight) {
    main_camera = std::make_unique<Camera>(glm::vec3(13.0f, 13.0f, 21.0f), glm::vec3(0.0f), _initialWidth, _initialHeight);

    default_shader = Shader::Library::CreateShader("shaders/default.vert", "shaders/default.frag");

    auto main_light_position = glm::vec3(10.0f, 0.0f, 13.0f);
    auto main_light_color = glm::vec3(0.99f, 0.95f, 0.78f);

    //grid
    Shader::Descriptor grid_s_descriptor = {
            .vertex_shader_path = "shaders/grid/grid.vert",
            .fragment_shader_path = "shaders/grid/grid.frag",
            .alpha = 0.4f
    };
    main_grid = std::make_unique<VisualGrid>(100, 100, 1.0f, glm::vec3(0.0f), glm::vec3(0.0f), grid_s_descriptor);

    //axis lines
    const char* line_vertex_shader_path = "shaders/line/line.vert";
    const char* line_fragment_shader_path = "shaders/line/line.frag";

    Shader::Descriptor x_line_s_descriptor = {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(1.0f, 0.0f, 0.0f)
    };

    Shader::Descriptor y_line_s_descriptor =  {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 1.0f, 0.0f)
    };

    Shader::Descriptor z_line_s_descriptor =  {
            .vertex_shader_path = line_vertex_shader_path,
            .fragment_shader_path = line_fragment_shader_path,
            .line_thickness = 3.0f,
            .color = glm::vec3(0.0f, 0.0f, 1.0f)
    };

    //this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_s_descriptor);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_s_descriptor);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_s_descriptor);

    //cube transform point offset (i.e. to scale it from the bottom-up)
    auto transform_offset = glm::vec3(0.0f, 0.0f, 0.5f);

    //cube instances are grouped by shader, since it's the only thing that differentiates one another
    //(everything else can be individually changed at any time)
    racket_cubes = std::vector<VisualCube>(5);
    const char* lit_vertex_shader_path = "shaders/lit/lit.vert";
    const char* lit_fragment_shader_path = "shaders/lit/lit.frag";

    const auto racket_line_thickness = 2.0f;
    const auto racket_point_size = 3.0f;

    racket_position = glm::vec3(0.0f);
    racket_rotation = glm::vec3(0.0f);
    racket_scale = glm::vec3(1.0f);

    Shader::Descriptor skin_s_descriptor =  {
            .vertex_shader_path = lit_vertex_shader_path,
            .fragment_shader_path = lit_fragment_shader_path,
            .line_thickness = racket_line_thickness,
            .point_size = racket_point_size,
            .color = glm::vec3(0.58f, 0.38f, 0.24f),
            .light_position = main_light_position,
            .light_color = main_light_color,
            .shininess = 2,
    };
    racket_cubes[0] = VisualCube(racket_position, racket_rotation, racket_scale, transform_offset, skin_s_descriptor); //skin

    Shader::Descriptor black_s_descriptor =  {
            .vertex_shader_path = lit_vertex_shader_path,
            .fragment_shader_path = lit_fragment_shader_path,
            .line_thickness = racket_line_thickness,
            .point_size = racket_point_size,
            .color = glm::vec3(0.2f),
            .light_position = main_light_position,
            .light_color = main_light_color,
            .shininess = 64,
    };
    racket_cubes[1] = VisualCube(racket_position, racket_rotation, racket_scale, transform_offset, black_s_descriptor); //racket handle (black plastic)

    Shader::Descriptor blue_s_descriptor =  {
            .vertex_shader_path = lit_vertex_shader_path,
            .fragment_shader_path = lit_fragment_shader_path,
            .line_thickness = racket_line_thickness,
            .point_size = racket_point_size,
            .color = glm::vec3(0.1f, 0.2f, 0.9f),
            .light_position = main_light_position,
            .light_color = main_light_color,
            .shininess = 64,
    };
    racket_cubes[2] = VisualCube(racket_position, racket_rotation, racket_scale, transform_offset, blue_s_descriptor); //racket piece (blue plastic)

    Shader::Descriptor green_s_descriptor =  {
            .vertex_shader_path = lit_vertex_shader_path,
            .fragment_shader_path = lit_fragment_shader_path,
            .line_thickness = racket_line_thickness,
            .point_size = racket_point_size,
            .color = glm::vec3(0.1f, 0.9f, 0.2f),
            .light_position = main_light_position,
            .light_color = main_light_color,
            .shininess = 64,
    };
    racket_cubes[3] = VisualCube(racket_position, racket_rotation, racket_scale, transform_offset, green_s_descriptor); //racket piece (green plastic)

    Shader::Descriptor white_s_descriptor =  {
            .vertex_shader_path = lit_vertex_shader_path,
            .fragment_shader_path = lit_fragment_shader_path,
            .line_thickness = racket_line_thickness,
            .point_size = racket_point_size,
            .color = glm::vec3(0.94f),
            .alpha = 0.95f,
            .light_position = main_light_position,
            .light_color = main_light_color,
            .shininess = 64,
    };
    racket_cubes[4] = VisualCube(racket_position, racket_rotation, racket_scale, transform_offset, white_s_descriptor); //racket net (white plastic)
}

void Renderer::Render(GLFWwindow* _window, const double _deltaTime) {
    //clears the canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //processes keyboard input
    InputCallback(_window, _deltaTime);

    //activates the default shader
    default_shader->Use();

    //draws the main grid
    main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    //draws the coordinate axis
    main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    //draws Augusto's racket
    DrawAugustoRacket(racket_position, racket_rotation, racket_scale);

    //draws Gab's racket
}

void Renderer::DrawAugustoRacket(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    //global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, racket_position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, racket_rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, racket_scale);

    //forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 5.0f));
    racket_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.2f));

    //arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 5.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-22.5f * sin(glfwGetTime()) - 22.5f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 4.0f));
    racket_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.25f));

    //racket handle (black plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 4.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 4.0f));
    racket_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.25f));

    //racket angled bottom left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 4.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 2.0f));
    racket_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.5f));

    //racket vertical left (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 2.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 3.0f));
    racket_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 3.0f));

    //racket angled top left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 3.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.0f));
    racket_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f));

    //racket horizontal top (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.6f));
    racket_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 1.6f));

    //racket angled top right (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.6f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.0f));
    racket_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f));

    //racket vertical right (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 3.0f));
    racket_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 3.0f));

    //racket horizontal bottom (blue plastic)
    auto horizontal_bottom_scale = glm::vec3(0.4f, 0.4f, 3.2f);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 3.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, horizontal_bottom_scale);
    racket_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / horizontal_bottom_scale);

    //racket net vertical (white plastic)

    //vertical net parts
    int number_of_same_nets_v = 4;
    auto net_first_v_translate = glm::vec3(0.0f, -0.65f, 0.0f);
    auto net_v_translate = glm::vec3(0.0f, -0.5f, 0.0f);
    auto net_v_scale = glm::vec3(0.1f, 0.1f, 3.55f);
    auto full_v_translate = net_first_v_translate + net_v_translate * (float)number_of_same_nets_v;

    //part 1
    //done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_v_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
    racket_cubes[4].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);

    //the rest of the net parts
    for (int i = 0; i < number_of_same_nets_v; ++i) {
        world_transform_matrix = glm::translate(world_transform_matrix, net_v_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
        racket_cubes[4].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);
    }

    //horizontal net parts
    int number_of_same_nets_h = 4;
    auto net_first_h_translate = glm::vec3(0.0f, -0.6f, 0.0f);
    auto net_h_translate = glm::vec3(0.0f, -0.5f, 0.0f);
    auto net_h_scale = glm::vec3(0.1f, 0.1f, 3.05f);
    auto full_h_translate = net_first_h_translate + net_h_translate * (float)number_of_same_nets_h;

    //correctly place the horizontal nets
    //the reason why it's a weird combination of y and z, is because we're always in relative space,
    //so depending on the current piece we're drawing, the orientation won't be the same
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -horizontal_bottom_scale.z - full_v_translate.y, 0.0f));

    //part 1
    //done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_h_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
    racket_cubes[4].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);

    //the rest of the net parts
    for (int i = 0; i < number_of_same_nets_h; ++i) {
        world_transform_matrix = glm::translate(world_transform_matrix, net_h_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
        racket_cubes[4].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);
    }

    //racket angled bottom right (blue plastic)
    //first we undo any transformations done for the net parts
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -full_v_translate.y, horizontal_bottom_scale.z));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(150.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 2.0f));
    racket_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.5f));

    //ball
    //transformed to be more or less in the corner of the net
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -horizontal_bottom_scale.z / 6.0f, horizontal_bottom_scale.z / 4.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-25.0f * glm::pow(glm::cos(glfwGetTime()), 2) + 26.5f, 3.0f, -1.5f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3((float)glm::cos(glfwGetTime()) * 360.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f));
    racket_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix, racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(1.0f));
}

void Renderer::ResizeCallback(GLFWwindow* _window, int _displayWidth, int _displayHeight) {
    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
}

void Renderer::InputCallback(GLFWwindow* _window, const double _deltaTime) {
    //keyboard triggers
    //render mode
    if (Input::IsKeyPressed(_window, GLFW_KEY_P))
        racket_render_mode = GL_POINTS;
    else if (Input::IsKeyPressed(_window, GLFW_KEY_L))
        racket_render_mode = GL_LINE_LOOP;
    else if (Input::IsKeyPressed(_window, GLFW_KEY_T))
        racket_render_mode = GL_TRIANGLES;

    //model transforms
    //translation
    if (Input::IsKeyReleased(_window, GLFW_KEY_SPACE)) //I know that std::rand isn't the best, but it'll do
        racket_position = glm::vec3(-50.0f + std::rand() / (float)RAND_MAX * 100.0f, -50.0f + std::rand() / (float)RAND_MAX * 100.0f, 0.0f);
    else if (Input::IsKeyReleased(_window, GLFW_KEY_TAB)) {
        racket_rotation = racket_position = glm::vec3(0.0f);
        racket_scale = glm::vec3(1.0f);
    }

    if (Input::IsKeyPressed(_window, GLFW_KEY_W) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(-1.0f, 0.0f, 0.0f) * (float)_deltaTime * 10.0f;
    if (Input::IsKeyPressed(_window, GLFW_KEY_S) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(1.0f, 0.0f, 0.0f) * (float)_deltaTime * 10.0f;
    if (Input::IsKeyPressed(_window, GLFW_KEY_A) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(0.0f, -1.0f, 0.0f) * (float)_deltaTime * 10.0f;
    if (Input::IsKeyPressed(_window, GLFW_KEY_D) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
        racket_position += glm::vec3(0.0f, 1.0f, 0.0f) * (float)_deltaTime * 10.0f;

    //rotation
    if (Input::IsKeyPressed(_window, GLFW_KEY_Q))
        racket_rotation += glm::vec3(-20.0f, 0.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_E))
        racket_rotation += glm::vec3(20.0f, 0.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_W))
        racket_rotation += glm::vec3(0.0f, 20.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_S))
        racket_rotation += glm::vec3(0.0f, -20.0f, 0.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_A))
        racket_rotation += glm::vec3(0.0f, 0.0f, 20.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_D))
        racket_rotation += glm::vec3(0.0f, 0.0f, -20.0f) * (float)_deltaTime;

    //scale
    if (Input::IsKeyPressed(_window, GLFW_KEY_U))
        racket_scale += glm::vec3(1.0f) * (float)_deltaTime;
    if (Input::IsKeyPressed(_window, GLFW_KEY_J))
        racket_scale += glm::vec3(-1.0f) * (float)_deltaTime;

    //camera translates (side to side and zoom forwards & back)
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_7)/* || Input::IsKeyPressed(_window, GLFW_KEY_Y)*/)
        main_camera->OneAxisMove(Camera::Translation::UP, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_9)/* || Input::IsKeyPressed(_window, GLFW_KEY_I)*/)
        main_camera->OneAxisMove(Camera::Translation::DOWN, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_4)/* || Input::IsKeyPressed(_window, GLFW_KEY_H)*/)
        main_camera->OneAxisMove(Camera::Translation::LEFT, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_6)/* || Input::IsKeyPressed(_window, GLFW_KEY_K)*/)
        main_camera->OneAxisMove(Camera::Translation::RIGHT, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_8) /*|| Input::IsKeyPressed(_window, GLFW_KEY_U)*/)
        main_camera->OneAxisMove(Camera::Translation::FORWARD, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_2) /*|| Input::IsKeyPressed(_window, GLFW_KEY_J)*/)
        main_camera->OneAxisMove(Camera::Translation::BACKWARD, (float)_deltaTime);

    //mouse triggers
    //forwards & back in camera local coordinates
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_LEFT))
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, (float)(_deltaTime * Input::cursor_delta_y));

    //tilt control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_MIDDLE))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_PITCH, (float)(_deltaTime * Input::cursor_delta_y));

    //pan control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_RIGHT))
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_YAW, (float) (_deltaTime * Input::cursor_delta_x));

    //camera rotation reset
    if (Input::IsKeyPressed(_window, GLFW_KEY_HOME) || Input::IsKeyPressed(_window, GLFW_KEY_KP_5))
        main_camera->SetDefaultPositionAndTarget();

    //keyboard triggers
    //camera orbit
    if (Input::IsKeyPressed(_window, GLFW_KEY_UP))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_UP, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_DOWN))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_DOWN, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_RIGHT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_RIGHT, (float)_deltaTime);
    if (Input::IsKeyPressed(_window, GLFW_KEY_LEFT))
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_LEFT, (float)_deltaTime);
}
