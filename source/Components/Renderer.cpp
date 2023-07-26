#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Transform.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight)
{
    main_camera = std::make_unique<Camera>(glm::vec3(0.0f, 25.0f, 30.0f), glm::vec3(0.0f), _initialWidth, _initialHeight);

    default_shader = Shader::Library::CreateShader("shaders/default.vert", "shaders/default.frag");

    const char *lit_vertex_shader_path = "shaders/lit/lit.vert";
    const char *lit_fragment_shader_path = "shaders/lit/lit.frag";

    // default material
    Shader::Descriptor default_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(1.0f)};

    // grid
    Shader::Descriptor grid_s_descriptor = {
        .vertex_shader_path = "shaders/grid/grid.vert",
        .fragment_shader_path = "shaders/grid/grid.frag",
        .alpha = 0.4f};
    main_grid = std::make_unique<VisualGrid>(78, 36, 1.0f, glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), grid_s_descriptor);

    const char *unlit_vertex_shader_path = "shaders/unlit/unlit.vert";
    const char *unlit_fragment_shader_path = "shaders/unlit/unlit.frag";

    // axis lines
    Shader::Descriptor x_line_s_descriptor = {
        .vertex_shader_path = unlit_vertex_shader_path,
        .fragment_shader_path = unlit_fragment_shader_path,
        .line_thickness = 3.0f,
        .color = glm::vec3(1.0f, 0.0f, 0.0f),
    };

    Shader::Descriptor y_line_s_descriptor = {
        .vertex_shader_path = unlit_vertex_shader_path,
        .fragment_shader_path = unlit_fragment_shader_path,
        .line_thickness = 3.0f,
        .color = glm::vec3(0.0f, 1.0f, 0.0f),
    };

    Shader::Descriptor z_line_s_descriptor = {
        .vertex_shader_path = unlit_vertex_shader_path,
        .fragment_shader_path = unlit_fragment_shader_path,
        .line_thickness = 3.0f,
        .color = glm::vec3(0.0f, 0.0f, 1.0f),
    };

    // this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_s_descriptor);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_s_descriptor);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_s_descriptor);

    // light
    // the way it currently is, isn't ideal, but it works for a quick demo
    // eventually, it will become its own component
    const auto light_position = glm::vec3(0.0f, 13.0f, 0.0f);
    const auto light_color = glm::vec3(0.99f, 0.95f, 0.78f);

    Shader::Descriptor sun_s_descriptor = {
        .vertex_shader_path = unlit_vertex_shader_path,
        .fragment_shader_path = unlit_fragment_shader_path,
        .color = light_color,
    };
    VisualCube sun_cube = VisualCube(light_position, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), sun_s_descriptor); // sun
    main_light = std::make_unique<Light>(sun_cube, light_position, light_color);

    // world cube
    Shader::Descriptor world_s_descriptor = {
        .vertex_shader_path = unlit_vertex_shader_path,
        .fragment_shader_path = unlit_fragment_shader_path,
        .color = glm::vec3(0.53f, 0.81f, 0.92f)};
    world_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(200.0f), glm::vec3(0.0f), world_s_descriptor);

    // cube transform point offset (i.e. to scale it from the bottom-up)
    auto bottom_y_transform_offset = glm::vec3(0.0f, 0.5f, 0.0f);

    // net
    net_cubes = std::vector<VisualCube>(2);

    Shader::Descriptor netpost_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.51f, 0.53f, 0.53f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 4,
    };
    net_cubes[0] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, netpost_s_descriptor); // net post

    Shader::Descriptor net_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.96f, 0.96f, 0.96f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 128,
    };
    net_cubes[1] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset); // net

    // letters
    letter_cubes = std::vector<VisualCube>(4);

    Shader::Descriptor p_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.15f, 0.92f, 0.17f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .ambient_strength = 0.2f,
        .shininess = 4,
    };
    letter_cubes[0] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, p_s_descriptor); // letter p

    Shader::Descriptor i_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.75f, 0.16f, 0.53f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .ambient_strength = 0.2f,
        .shininess = 128,
    };
    letter_cubes[1] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, i_s_descriptor); // letter i

    Shader::Descriptor n_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.34f, 0.84f, 0.98f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .ambient_strength = 0.2f,
        .shininess = 128,
    };
    letter_cubes[2] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, n_s_descriptor); // letter n

    Shader::Descriptor h_s_descriptor = {
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .color = glm::vec3(0.92f, 0.16f, 0.13f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .ambient_strength = 0.2f,
        .shininess = 128,
    };
    letter_cubes[3] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, h_s_descriptor); // letter h

    // cube transform point offset (i.e. to scale it from the bottom-up)
    auto bottom_z_transform_offset = glm::vec3(0.0f, 0.0f, 0.5f);

    const auto racket_line_thickness = 2.0f;
    const auto racket_point_size = 3.0f;

    // augusto racket cube + materials
    augusto_racket_cube = std::make_shared<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_z_transform_offset, default_s_descriptor);
    augusto_racket_materials = std::vector<Shader::Descriptor>();

    rackets = std::vector<Racket>(3);
    default_rackets = std::vector<Racket>(3);

    augusto_racket_materials.push_back({
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.58f, 0.38f, 0.24f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 2,
    }); // skin

    augusto_racket_materials.push_back({
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.2f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 64,
    }); // racket handle (black plastic)

    augusto_racket_materials.push_back({
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.1f, 0.2f, 0.9f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 64,
    }); // racket piece (blue plastic)

    augusto_racket_materials.push_back({
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.1f, 0.9f, 0.2f),
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 64,
    }); // racket piece (green plastic)

    augusto_racket_materials.push_back({
        .vertex_shader_path = lit_vertex_shader_path,
        .fragment_shader_path = lit_fragment_shader_path,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.94f),
        .alpha = 0.95f,
        .light_position = main_light->position,
        .light_color = main_light->color,
        .shininess = 64,
    }); // racket net (white plastic)

    // augusto's racket position
    rackets[0] = default_rackets[0] = Racket(
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f),
            glm::vec3(0.8f));

    ////

    // gabrielle racket cube
    gabrielle_racket_cube = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_z_transform_offset, default_s_descriptor);
    rackets[1] = default_rackets[1] = Racket(
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.8f));
    ////

    // jack racket cube
    jack_racket_cube = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_z_transform_offset, default_s_descriptor);
    rackets[2] = default_rackets[0] = Racket(
        glm::vec3(-10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.8f));
    //
}

void Renderer::Render(GLFWwindow *_window, const double _deltaTime)
{
    // clears the canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // processes keyboard input
    InputCallback(_window, _deltaTime);

    // activates the default shader
    default_shader->Use();

    // draws the world cube
    world_cube->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the main grid
    main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the coordinate axis
    main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the sun
    main_light->cube.Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the net
    DrawOneNet(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    // draws the rackets
    DrawOneAugustoRacket(rackets[0].position, rackets[0].rotation, rackets[0].scale);
    DrawOneGabrielleRacket(rackets[1].position, rackets[1].rotation, rackets[1].scale);
    DrawOneJackRacket(rackets[2].position, rackets[2].rotation, rackets[2].scale);
}

void Renderer::DrawOneNet(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, scale);

    auto scale_factor = glm::vec3(0.0f);

    // first net post
    scale_factor = glm::vec3(1.0f, 8.0f, 1.0f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -18.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    net_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // horizontal net
    auto h_net_count = 7;
    scale_factor = glm::vec3(0.2f, 36.0f, 0.2f);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));

    for (int i = 0; i < h_net_count; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        net_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    }

    // vertical net
    auto v_net_count = 36;
    scale_factor = glm::vec3(0.2f, (float)(h_net_count - 1), 0.2f);

    // undo any movement from the horizontal net
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, (float)(h_net_count - 1)));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));

    for (int i = 0; i < v_net_count; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        net_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    }

    // second net post
    scale_factor = glm::vec3(1.0f, 8.0f, 1.0f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    net_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::DrawOneAugustoRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, scale);

    // forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 5.0f, 1.0f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[0]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.2f, 1.0f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-22.5f * sin(glfwGetTime()) - 22.5f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[0]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.25f, 1.0f));

    // racket handle (black plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 4.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[1]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.25f, 2.0f));

    // racket angled bottom left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.5f, 2.0f));

    // racket vertical left (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 3.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[3]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 3.0f, 2.0f));

    // racket angled top left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f, 2.0f));

    // racket horizontal top (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.6f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[3]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 1.6f, 2.0f));

    // racket angled top right (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.6f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f, 2.0f));

    // racket vertical right (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 3.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[3]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 3.0f, 2.0f));

    // racket horizontal bottom (blue plastic)
    auto horizontal_bottom_scale = glm::vec3(0.4f, 0.4f, 3.2f);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, horizontal_bottom_scale);
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / horizontal_bottom_scale);

    // racket net vertical (white plastic)

    // vertical net parts
    int number_of_same_nets_v = 4;
    auto net_first_v_translate = glm::vec3(-0.65f, 0.0f, 0.0f);
    auto net_v_translate = glm::vec3(-0.5f, 0.0f, 0.0f);
    auto net_v_scale = glm::vec3(0.1f, 3.55f, 0.1f);
    auto full_v_translate = net_first_v_translate + net_v_translate * (float)number_of_same_nets_v;

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_v_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_v; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_v_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
        augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(),
                                       world_transform_matrix, racket_render_mode);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);
    }

    // horizontal net parts
    int number_of_same_nets_h = 4;
    auto net_first_h_translate = glm::vec3(-0.6f, 0.0f, 0.0f);
    auto net_h_translate = glm::vec3(-0.5f, 0.0f, 0.0f);
    auto net_h_scale = glm::vec3(0.1f, 3.05f, 0.1f);
    auto full_h_translate = net_first_h_translate + net_h_translate * (float)number_of_same_nets_h;

    // correctly place the horizontal nets
    // the reason why it's a weird combination of y and z, is because we're always in relative space,
    // so depending on the current piece we're drawing, the orientation won't be the same
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(0.0f, -horizontal_bottom_scale.z - full_v_translate.y, 0.0f));

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_h_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[4]);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_h; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_h_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
        augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(),
                                       world_transform_matrix, racket_render_mode, &augusto_racket_materials[4]);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);
    }

    // racket angled bottom right (blue plastic)
    // first we undo any transformations done for the net parts
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(0.0f, -full_v_translate.y, horizontal_bottom_scale.z));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(150.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[2]);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.5f, 2.0f));

    // ball
    // transformed to be more or less in the corner of the net
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -horizontal_bottom_scale.z / 6.0f,
                                                                              horizontal_bottom_scale.z / 4.0f));
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(-25.0f * glm::pow(glm::cos(glfwGetTime()), 2) + 26.5f, 3.0f,
                                                      -1.5f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix,
                                                       glm::vec3((float)glm::cos(glfwGetTime()) * 360.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f));
    augusto_racket_cube->DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode, &augusto_racket_materials[3]);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(1.0f));
}

void Renderer::DrawOneGabrielleRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, scale);

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // forearm (skin)
    gabrielle_racket_cube.shader_descriptor.color = glm::vec3(0.871f, 0.722f, 0.529f);

    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(135.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 5.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                         racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.2f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 5.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-22.5f * sin(glfwGetTime()) - 22.5f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 4.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                         racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.25f));

    // racket handle (black plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 4.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 4.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.25f));

    // racket angled bottom left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 4.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 2.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.5f));

    // racket vertical left (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 2.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 3.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 3.0f));

    // racket angled top left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 3.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f));

    // racket horizontal top (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.6f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 1.6f));

    // racket angled top right (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.6f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 1.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f));

    // racket vertical right (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 3.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 1.0f / 3.0f));

    // racket horizontal bottom (blue plastic)
    auto horizontal_bottom_scale = glm::vec3(0.4f, 0.4f, 3.2f);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 3.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, horizontal_bottom_scale);
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / horizontal_bottom_scale);

    // racket net vertical (white plastic)

    // vertical net parts
    int number_of_same_nets_v = 4;
    auto net_first_v_translate = glm::vec3(0.0f, -0.65f, 0.0f);
    auto net_v_translate = glm::vec3(0.0f, -0.5f, 0.0f);
    auto net_v_scale = glm::vec3(0.1f, 0.1f, 3.55f);
    auto full_v_translate = net_first_v_translate + net_v_translate * (float)number_of_same_nets_v;

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_v_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_v; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_v_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
        gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(),
                                       world_transform_matrix, racket_render_mode);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);
    }

    // horizontal net parts
    int number_of_same_nets_h = 4;
    auto net_first_h_translate = glm::vec3(0.0f, -0.6f, 0.0f);
    auto net_h_translate = glm::vec3(0.0f, -0.5f, 0.0f);
    auto net_h_scale = glm::vec3(0.1f, 0.1f, 3.05f);
    auto full_h_translate = net_first_h_translate + net_h_translate * (float)number_of_same_nets_h;

    // correctly place the horizontal nets
    // the reason why it's a weird combination of y and z, is because we're always in relative space,
    // so depending on the current piece we're drawing, the orientation won't be the same
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(0.0f, -horizontal_bottom_scale.z - full_v_translate.y, 0.0f));

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_h_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_h; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_h_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
        gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(),
                                       world_transform_matrix, racket_render_mode);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);
    }

    // racket angled bottom right (blue plastic)
    // first we undo any transformations done for the net parts
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(0.0f, -full_v_translate.y, horizontal_bottom_scale.z));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(150.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 2.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.5f));

    // ball
    // transformed to be more or less in the corner of the net
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -horizontal_bottom_scale.z / 6.0f,
                                                                              horizontal_bottom_scale.z / 4.0f));
    world_transform_matrix = glm::translate(world_transform_matrix,
                                            glm::vec3(-25.0f * glm::pow(glm::cos(glfwGetTime()), 2) + 26.5f, 3.0f,
                                                      -1.5f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix,
                                                       glm::vec3((float)glm::cos(glfwGetTime()) * 360.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f));
    gabrielle_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(1.0f));
}

void Renderer::DrawOneJackRacket(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    // 1.000f, 0.894f, 0.769f
    // global transforms
    // his position is my xyz in vec
    // his rotation for me is a y vector
    // his scale is like mine

    world_transform_matrix = glm::translate(world_transform_matrix, position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, scale);

    glm::mat4 secondary_transform_matrix = world_transform_matrix;

    // forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(135.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 5.0f));
    jack_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.2f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 5.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-22.5f * sin(glfwGetTime()) - 22.5f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 4.0f));
    jack_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.25f));

    // racket handle (black plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 4.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 0.5f, 4.0f));
    jack_racket_cube.DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix,
                                   racket_render_mode);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 2.0f, 0.25f));
}

void Renderer::DrawOneP(glm::mat4 world_transform_matrix)
{
    // long P vertical
    auto scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // short top P horizontal
    scale_factor = glm::vec3(0.5f, 3.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f)); // translate to the end of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // short right P vertical
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f)); // translate to the end of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // short bottom P horizontal
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f)); // translate to the end of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::DrawOneI(glm::mat4 world_transform_matrix)
{
    // short I bottom horizontal
    auto scale_factor = glm::vec3(0.5f, 3.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.5f, 20.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // long I vertical
    scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.5f, 0.0f)); // translate to the middle of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, -90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // short I top horizontal
    scale_factor = glm::vec3(0.5f, 3.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.5f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::DrawOneN(glm::mat4 world_transform_matrix)
{
    // long N vertical
    auto scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // long N diagonal
    scale_factor = glm::vec3(0.5f, 7.07f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f)); // translate to the end of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, -135.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // long N vertical
    scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 7.07f, 0.0f)); // translate to the end of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 135.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::DrawOneH(glm::mat4 world_transform_matrix)
{
    // long H left vertical
    auto scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.5f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // short H middle horizontal
    scale_factor = glm::vec3(0.5f, 3.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.5f, 0.0f)); // translate to the middle of the previous cube
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // long H right vertical
    scale_factor = glm::vec3(0.5f, 5.0f, 0.5f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(2.5f, 3.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, -90.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[3].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), world_transform_matrix);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::ResizeCallback(GLFWwindow *_window, int _displayWidth, int _displayHeight)
{
    main_camera->SetViewportSize((float)_displayWidth, (float)_displayHeight);
}

void Renderer::InputCallback(GLFWwindow *_window, const double _deltaTime)
{
    if (Input::IsKeyPressed(_window, GLFW_KEY_1))
    {
        selected_player = 0;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_2))
    {
        selected_player = 1;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_3))
    {
        selected_player = 2;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_4))
    {
        selected_player = 3;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_5))
    {
        selected_player = 4;
    }

    const int *desired_keys = new int[5]{GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5};
    if (Input::IsAnyKeyPressed(_window, 5, desired_keys))
    {
        // sets focus on the selected player
        main_camera->SetPosition(rackets[selected_player].position + glm::vec3(0.0f, 25.0f, 30.0f));
        main_camera->SetTarget(rackets[selected_player].position);
    }

    // keyboard triggers
    // render mode
    if (Input::IsKeyPressed(_window, GLFW_KEY_P))
    {
        racket_render_mode = GL_POINTS;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_L))
    {
        racket_render_mode = GL_LINE_LOOP;
    }
    else if (Input::IsKeyPressed(_window, GLFW_KEY_T))
    {
        racket_render_mode = GL_TRIANGLES;
    }

    // model transforms
    // translation
    if (Input::IsKeyReleased(_window, GLFW_KEY_TAB))
    {
        rackets[selected_player].position = default_rackets[selected_player].position;
        rackets[selected_player].rotation = default_rackets[selected_player].rotation;
        rackets[selected_player].scale = default_rackets[selected_player].scale;
    }

    if (Input::IsKeyPressed(_window, GLFW_KEY_W) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
    {
        rackets[selected_player].position += glm::vec3(-1.0f, 0.0f, 0.0f) * (float)_deltaTime * 10.0f;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_S) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
    {
        rackets[selected_player].position += glm::vec3(1.0f, 0.0f, 0.0f) * (float)_deltaTime * 10.0f;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_A) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
    {
        rackets[selected_player].position += glm::vec3(0.0f, -1.0f, 0.0f) * (float)_deltaTime * 10.0f;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_D) && Input::IsKeyPressed(_window, GLFW_KEY_LEFT_SHIFT))
    {
        rackets[selected_player].position += glm::vec3(0.0f, 1.0f, 0.0f) * (float)_deltaTime * 10.0f;
    }

    // rotation
    if (Input::IsKeyPressed(_window, GLFW_KEY_Q))
    {
        rackets[selected_player].rotation += glm::vec3(-20.0f, 0.0f, 0.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_E))
    {
        rackets[selected_player].rotation += glm::vec3(20.0f, 0.0f, 0.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_W))
    {
        rackets[selected_player].rotation += glm::vec3(0.0f, 0.0f, 20.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_S))
    {
        rackets[selected_player].rotation += glm::vec3(0.0f, 0.0f, -20.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_A))
    {
        rackets[selected_player].rotation += glm::vec3(0.0f, 20.0f, 0.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_D))
    {
        rackets[selected_player].rotation += glm::vec3(0.0f, -20.0f, 0.0f) * (float)_deltaTime;
    }

    // scale
    if (Input::IsKeyPressed(_window, GLFW_KEY_U))
    {
        rackets[selected_player].scale += glm::vec3(1.0f) * (float)_deltaTime;
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_J))
    {
        rackets[selected_player].scale += glm::vec3(-1.0f) * (float)_deltaTime;
    }

    // camera translates (side to side and zoom forwards & back)
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_7) /* || Input::IsKeyPressed(_window, GLFW_KEY_Y)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::UP, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_9) /* || Input::IsKeyPressed(_window, GLFW_KEY_I)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::DOWN, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_4) /* || Input::IsKeyPressed(_window, GLFW_KEY_H)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::LEFT, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_6) /* || Input::IsKeyPressed(_window, GLFW_KEY_K)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::RIGHT, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_8) /*|| Input::IsKeyPressed(_window, GLFW_KEY_U)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::FORWARD, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_KP_2) /*|| Input::IsKeyPressed(_window, GLFW_KEY_J)*/)
    {
        main_camera->OneAxisMove(Camera::Translation::BACKWARD, (float)_deltaTime);
    }

    // mouse triggers
    // forwards & back in camera local coordinates
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_LEFT))
    {
        main_camera->OneAxisMove(Camera::Translation::CAMERA_FORWARD, (float)(_deltaTime * Input::cursor_delta_y));
    }

    // tilt control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_MIDDLE))
    {
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_PITCH, (float)(_deltaTime * Input::cursor_delta_y));
    }

    // pan control
    if (Input::IsMouseButtonPressed(_window, GLFW_MOUSE_BUTTON_RIGHT))
    {
        main_camera->OneAxisRotate(Camera::Rotation::POSITIVE_YAW, (float)(_deltaTime * Input::cursor_delta_x));
    }

    // camera rotation reset
    if (Input::IsKeyPressed(_window, GLFW_KEY_HOME) || Input::IsKeyPressed(_window, GLFW_KEY_KP_5))
    {
        main_camera->SetDefaultPositionAndTarget();
    }

    // keyboard triggers
    // camera orbit
    if (Input::IsKeyPressed(_window, GLFW_KEY_UP))
    {
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_UP, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_DOWN))
    {
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_DOWN, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_RIGHT))
    {
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_RIGHT, (float)_deltaTime);
    }
    if (Input::IsKeyPressed(_window, GLFW_KEY_LEFT))
    {
        main_camera->OneAxisOrbit(Camera::Orbitation::ORBIT_LEFT, (float)_deltaTime);
    }
}
