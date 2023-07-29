#include "Renderer.h"
#include "Utility/Input.hpp"
#include "Utility/Transform.hpp"

Renderer::Renderer(int _initialWidth, int _initialHeight)
{
    viewport_width = _initialWidth;
    viewport_height = _initialHeight;

    main_camera = std::make_unique<Camera>(glm::vec3(0.0f, 25.0f, 30.0f), glm::vec3(0.0f), viewport_width, viewport_height);

    main_light = std::make_unique<Light>(glm::vec3(0.0f, 13.0f, 0.0f), glm::vec3(0.99f, 0.95f, 0.78f), 0.2f, 0.4f);

    auto grid_shader = Shader::Library::CreateShader("shaders/grid/grid.vert", "shaders/grid/grid.frag");
    auto unlit_shader = Shader::Library::CreateShader("shaders/unlit/unlit.vert", "shaders/unlit/unlit.frag");
    auto lit_shader = Shader::Library::CreateShader("shaders/lit/lit.vert", "shaders/lit/lit.frag");

    auto shadow_mapper_shader = Shader::Library::CreateShader("shaders/shadows/shadow_mapper.vert", "shaders/shadows/shadow_mapper.frag");
    auto screen_shader = Shader::Library::CreateShader("shaders/screen/screen.vert", "shaders/screen/screen.frag");

    shadow_mapper_material = std::make_unique<Shader::Material>();
    shadow_mapper_material->shader = shadow_mapper_shader;

    Shader::Material main_light_cube_material = {
        .shader = unlit_shader,
        .color = main_light->GetColor(),
        .main_light = main_light,
    };
    main_light_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), main_light_cube_material);

    Shader::Material screen_material = {
        .shader = screen_shader,
    };
    main_screen = std::make_unique<Screen>(screen_material);

    // default material
    Shader::Material default_s_material = {
        .shader = lit_shader,
        .color = glm::vec3(1.0f),
        .main_light = main_light,
    };

    // grid
    Shader::Material grid_s_material = {
        .shader = grid_shader,
        .alpha = 0.4f,
    };
    main_grid = std::make_unique<VisualGrid>(78, 36, 1.0f, glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), grid_s_material);

    // axis lines
    Shader::Material x_line_s_material = {
        .shader = unlit_shader,
        .line_thickness = 3.0f,
        .color = glm::vec3(1.0f, 0.0f, 0.0f),
    };

    Shader::Material y_line_s_material = {
        .shader = unlit_shader,
        .line_thickness = 3.0f,
        .color = glm::vec3(0.0f, 1.0f, 0.0f),
    };

    Shader::Material z_line_s_material = {
        .shader = unlit_shader,
        .line_thickness = 3.0f,
        .color = glm::vec3(0.0f, 0.0f, 1.0f),
    };

    // this is a quick way to make the axis lines avoid having depth fighting issues
    main_x_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(5.01f, 0.01f, 0.01f), x_line_s_material);
    main_y_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 5.01f, 0.01f), y_line_s_material);
    main_z_line = std::make_unique<VisualLine>(glm::vec3(0.01f), glm::vec3(0.01f, 0.01f, 5.01f), z_line_s_material);

    // world cube
    Shader::Material world_s_material = {
        .shader = unlit_shader,
        .color = glm::vec3(0.53f, 0.81f, 0.92f),
    };
    world_cube = std::make_unique<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(200.0f), glm::vec3(0.0f), world_s_material);

    // cube transform point offset (i.e. to scale it from the bottom-up)
    auto bottom_y_transform_offset = glm::vec3(0.0f, 0.5f, 0.0f);

    // net
    net_cubes = std::vector<VisualCube>(2);

    Shader::Material netpost_s_material = {
        .shader = lit_shader,
        .color = glm::vec3(0.51f, 0.53f, 0.53f),
        .main_light = main_light,
        .shininess = 4,
    };
    net_cubes[0] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, netpost_s_material); // net post

    Shader::Material net_s_material = {
        .shader = lit_shader,
        .color = glm::vec3(0.96f, 0.96f, 0.96f),
        .main_light = main_light,
        .shininess = 128,
    };
    net_cubes[1] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, net_s_material); // net

    // letters
    letter_cubes = std::vector<VisualCube>(4);

    Shader::Material a_s_material = {
        .shader = lit_shader,
        .color = glm::vec3(0.15f, 0.92f, 0.17f),
        .main_light = main_light,
        .shininess = 4,
    };
    letter_cubes[0] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, a_s_material); // letter a

    letter_cubes[1] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, default_s_material); // letter g

    letter_cubes[2] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, default_s_material); // letter j

    Shader::Material j_s_material = {
        .shader = lit_shader,
        .color = glm::vec3(0.34f, 0.84f, 0.98f),
        .main_light = main_light,
        .shininess = 128,
    };
    letter_cubes[2] = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, j_s_material); // letter j

    const auto racket_line_thickness = 2.0f;
    const auto racket_point_size = 3.0f;

    // augusto racket cube + materials
    augusto_racket_cube = std::make_shared<VisualCube>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, default_s_material);
    augusto_racket_materials = std::vector<Shader::Material>();

    rackets = std::vector<Racket>(3);
    default_rackets = std::vector<Racket>(3);

    augusto_racket_materials.push_back({
        .shader = lit_shader,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.58f, 0.38f, 0.24f),
        .main_light = main_light,
        .shininess = 2,
    }); // skin

    augusto_racket_materials.push_back({
        .shader = lit_shader,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.2f),
        .main_light = main_light,
        .shininess = 64,
    }); // racket handle (black plastic)

    augusto_racket_materials.push_back({
        .shader = lit_shader,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.1f, 0.2f, 0.9f),
        .main_light = main_light,
        .shininess = 64,
    }); // racket piece (blue plastic)

    augusto_racket_materials.push_back({
        .shader = lit_shader,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.1f, 0.9f, 0.2f),
        .main_light = main_light,
        .shininess = 64,
    }); // racket piece (green plastic)

    augusto_racket_materials.push_back({
        .shader = lit_shader,
        .line_thickness = racket_line_thickness,
        .point_size = racket_point_size,
        .color = glm::vec3(0.94f),
        .alpha = 0.95f,
        .main_light = main_light,
        .shininess = 64,
    }); // racket net (white plastic)

    // augusto's racket position
    rackets[0] = default_rackets[0] = Racket(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.8f));

    ////

    // gabrielle racket cube
    gabrielle_racket_cube = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, default_s_material);
    rackets[1] = default_rackets[1] = Racket(
        glm::vec3(10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.8f));

    ////

    // jack racket cube
    jack_racket_cube = VisualCube(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), bottom_y_transform_offset, default_s_material);
    rackets[2] = default_rackets[2] = Racket(
        glm::vec3(-10.0f, 0.0f, 0.0f),
        glm::vec3(0.0f),
        glm::vec3(0.8f));
    //
}

void Renderer::Init() {
    // initializes the shadow map framebuffer
    glGenFramebuffers(1, &shadow_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);

    // cleanup the texture bind
    glBindTexture(GL_TEXTURE_2D, 0);

    // initializes the shadow map depth texture
    glGenTextures(1, &shadow_map_depth_tex);
    glBindTexture(GL_TEXTURE_2D, shadow_map_depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // binds the shadow map depth texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_depth_tex, 0);

    // cleanup the texture bind
    glBindTexture(GL_TEXTURE_2D, 0);

    // disable color draw buffer
    glDrawBuffer(GL_NONE);

    // checks if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR -> Framebuffer is not complete!" << std::endl;

    // unbinds the current framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Render(GLFWwindow *_window, const double _deltaTime)
{
    // processes input
    InputCallback(_window, _deltaTime);

    // moves the main light
    auto light_turning_radius = 4.0f;
    main_light->SetPosition(glm::vec3(glm::cos(glfwGetTime() * 2.0f) * light_turning_radius, 10.0f * glm::sin(glfwGetTime() / 2.0f) + 10.0f, glm::sin(glfwGetTime()) *  light_turning_radius));

    // SHADOW MAP PASS

    // binds the shadow map framebuffer to draw on it
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glViewport(0, 0, Light::LIGHTMAP_SIZE, Light::LIGHTMAP_SIZE);
    glBindTexture(GL_TEXTURE_2D, shadow_map_depth_tex);

    // clears the color & depth canvas to black
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 quick_floor_transform_matrix = glm::mat4(1.0f);
    quick_floor_transform_matrix = Transforms::RotateDegrees(quick_floor_transform_matrix, glm::vec3(180.0f, 0.0f, 0.0f));
    quick_floor_transform_matrix = glm::scale(quick_floor_transform_matrix, glm::vec3(10.0f));

    //quick shadow catcher test
    net_cubes[0].DrawFromMatrix(main_light->GetViewProjection(), main_light->GetPosition(), quick_floor_transform_matrix, GL_TRIANGLES, shadow_mapper_material.get());

    // draws the net
    DrawOneNet(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), main_light->GetViewProjection(), main_light->GetPosition(), shadow_mapper_material.get());

    // draws the rackets
    DrawOneAugustoRacket(rackets[0].position, rackets[0].rotation, rackets[0].scale, main_light->GetViewProjection(), main_light->GetPosition(), shadow_mapper_material.get());
    DrawOneGabrielleRacket(rackets[1].position, rackets[1].rotation, rackets[1].scale, main_light->GetViewProjection(), main_light->GetPosition(), shadow_mapper_material.get());
    DrawOneJackRacket(rackets[2].position, rackets[2].rotation, rackets[2].scale, main_light->GetViewProjection(), main_light->GetPosition(), shadow_mapper_material.get());

    // unbind the current framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // COLOR PASS

    // resets the viewport to the window size
    glViewport(0, 0, viewport_width, viewport_height);

    // activates the shadow map depth texture & binds it to the second texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadow_map_depth_tex);

    // clears the color & depth canvas to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //quick shadow catcher test
    net_cubes[0].DrawFromMatrix(main_camera->GetViewProjection(), main_camera->GetPosition(), quick_floor_transform_matrix);

    // draws the world cube
    world_cube->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the main light cube
    main_light_cube->position = main_light->GetPosition();
    main_light_cube->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the main grid
    main_grid->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the coordinate axis
    main_x_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_y_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());
    main_z_line->Draw(main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the net
    DrawOneNet(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), main_camera->GetViewProjection(), main_camera->GetPosition());

    // draws the rackets
    DrawOneAugustoRacket(rackets[0].position, rackets[0].rotation, rackets[0].scale, main_camera->GetViewProjection(), main_camera->GetPosition());
    DrawOneGabrielleRacket(rackets[1].position, rackets[1].rotation, rackets[1].scale, main_camera->GetViewProjection(), main_camera->GetPosition());
    DrawOneJackRacket(rackets[2].position, rackets[2].rotation, rackets[2].scale, main_camera->GetViewProjection(), main_camera->GetPosition());

    //main_screen->Draw();
}

void Renderer::DrawOneNet(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    auto scale_factor = glm::vec3(0.0f);

    // first net post
    scale_factor = glm::vec3(1.0f, 8.0f, 1.0f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -18.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    net_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    // horizontal net
    auto h_net_count = 7;
    scale_factor = glm::vec3(0.2f, 36.0f, 0.2f);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));

    for (int i = 0; i < h_net_count; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
        net_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,  GL_TRIANGLES, _materialOverride);
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
        net_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
    }

    // second net post
    scale_factor = glm::vec3(1.0f, 8.0f, 1.0f);
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    net_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::DrawOneAugustoRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    // letter A
    DrawOneA(world_transform_matrix, _viewProjection, _eyePosition, _materialOverride);

    // forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 5.0f, 1.0f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                           nullptr ? &augusto_racket_materials[0] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.2f, 1.0f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-22.5f * sin(glfwGetTime()) - 22.5f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[0] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.25f, 1.0f));

    // racket handle (black plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 4.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[1] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.25f, 2.0f));

    // racket angled bottom left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[2] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.5f, 2.0f));

    // racket vertical left (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 3.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[3] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 3.0f, 2.0f));

    // racket angled top left (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[2] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f, 2.0f));

    // racket horizontal top (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.6f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[3] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 1.6f, 2.0f));

    // racket angled top right (blue plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.6f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[2] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f, 2.0f));

    // racket vertical right (green plastic)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 3.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[3] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1.0f / 3.0f, 2.0f));

    // racket horizontal bottom (blue plastic)
    auto horizontal_bottom_scale = glm::vec3(0.4f, 3.2f, 0.4f);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 3.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, horizontal_bottom_scale);
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[2] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / horizontal_bottom_scale);

    // racket net vertical (white plastic)

    // vertical net parts
    int number_of_same_nets_v = 4;
    auto net_first_v_translate = glm::vec3(-0.65f, 0.0f, 0.0f);
    auto net_v_translate = glm::vec3(-0.5f, 0.0f, 0.0f);
    auto net_v_scale = glm::vec3(0.1f, 3.55f, 0.1f);
    auto full_v_translate = net_first_v_translate + net_v_translate * (float)number_of_same_nets_v;

    // correct orientation for the vertical nets
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, -90.0f, 0.0f));

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_v_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[4] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_v; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_v_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_v_scale);
        augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                       nullptr ? &augusto_racket_materials[4] : _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_v_scale);
    }

    // horizontal net parts
    int number_of_same_nets_h = 4;
    auto net_first_h_translate = glm::vec3(-0.6f, 0.0f, 0.0f);
    auto net_h_translate = glm::vec3(-0.5f, 0.0f, 0.0f);
    auto net_h_scale = glm::vec3(0.1f, 3.05f, 0.1f);
    auto full_h_translate = net_first_h_translate + net_h_translate * (float)number_of_same_nets_h;

    // correctly place and rotate the horizontal nets
    // the reason why it's a weird combination of y and z, is because we're always in relative space,
    // so depending on the current piece we're drawing, the orientation won't be the same
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-horizontal_bottom_scale.z - full_v_translate.y, 0.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 90.0f));

    // part 1
    // done separately because it has a different offset (for aesthetic purposes)
    world_transform_matrix = glm::translate(world_transform_matrix, net_first_h_translate);
    world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[4] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);

    // the rest of the net parts
    for (int i = 0; i < number_of_same_nets_h; ++i)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, net_h_translate);
        world_transform_matrix = glm::scale(world_transform_matrix, net_h_scale);
        augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                       nullptr ? &augusto_racket_materials[4] : _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / net_h_scale);
    }

    // racket angled bottom right (blue plastic)
    // first we undo any transformations done for the net parts
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-full_v_translate.x, horizontal_bottom_scale.y, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 150.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.0f, 0.5f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[2] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.5f, 2.0f));

    // ball
    // transformed to be more or less in the center of the net
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-full_v_translate.x / 2.0f, horizontal_bottom_scale.y / 2.0f, 0.0f));
    // transformed to animate back and forth
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.5f, -25.0f * glm::pow(glm::cos(glfwGetTime()), 2) + 25.5f));

    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3((float)glm::cos(glfwGetTime()) * 360.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f));
    augusto_racket_cube->DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride ==
                                                                                                                   nullptr ? &augusto_racket_materials[3] : _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / glm::vec3(1.0f));
}

void Renderer::DrawOneGabrielleRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);
    // global transforms
    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    // draw letter G //
    glm::mat4 secondary_transform_matrix = world_transform_matrix;
    DrawOneG(secondary_transform_matrix, _viewProjection, _eyePosition, _materialOverride);

    // arm //
    gabrielle_racket_cube.material.color = glm::vec3(0.871f, 0.722f, 0.529f); // skin colour

    // forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 5.0f, 1.0f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.2f, 1.0f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.25f, 1.0f));

    // racket //
    gabrielle_racket_cube.material.color = glm::vec3(1.0f, 0.714f, 0.757f); // pink colour

    // racket handle
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 4.0f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.25f, 2.0f));

    // racket angled bottom left
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.5f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.4f, 2.0f));

    // racket vertical left
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.5f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1 / 2.5f, 2.0f));

    // racket angle top left
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(40.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.5f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1 / 1.5f, 2.0f));

    // racket horizontal top
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(50.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.0f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.5f, 2.0f));

    // racket angle top right
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(50.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 1.5f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1 / 1.5f, 2.0f));

    // racket vertical right
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(40.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.5f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1 / 2.5f, 2.0f));

    // racket horizontal bottom
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 4.0f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 1 / 4.0f, 2.0f));

    // racket angled bottom right
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-150.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2.5f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-60.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 2.25f, 0.5f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1 / 0.5f, 1 / 2.25f, 1 / 0.5f));

    // net //
    gabrielle_racket_cube.material.color = glm::vec3(1.0f, 1.0f, 1.0f); // white net colour

    // setup for nets horizontal
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 5.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(30.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 2.5f, 0.1f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1 / 0.1f, 1 / 2.5f, 1 / 0.1f));

    for (float j = 0; j < 6; j++)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
        world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 3.5f, 0.1f));
        gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                             racket_render_mode, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1 / 0.1f, 1 / 3.5f, 1 / 0.1f));
    }

    // setup for nets vertical
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 3.5f, 0.1f));
    gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                         racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1 / 0.1f, 1 / 3.5f, 1 / 0.1f));

    for (float j = 0; j < 6; j++)
    {
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
        world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 3.5f, 0.1f));
        gabrielle_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                             racket_render_mode, _materialOverride);
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1 / 0.1f, 1 / 3.5f, 1 / 0.1f));
    }
}

void Renderer::DrawOneJackRacket(const glm::vec3 &_position, const glm::vec3 &_rotation, const glm::vec3 &_scale, const glm::mat4& _viewProjection, const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    glm::mat4 world_transform_matrix = glm::mat4(1.0f);

    // 1.000f, 0.894f, 0.769f
    // global transforms
    // his position is my xyz in vec
    // his rotation for me is a y vector
    // his scale is like mine

    world_transform_matrix = glm::translate(world_transform_matrix, _position);
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, _rotation);
    world_transform_matrix = glm::scale(world_transform_matrix, _scale);

    // draw letter J //
    glm::mat4 secondary_transform_matrix = world_transform_matrix;
    DrawOneJ(secondary_transform_matrix, _viewProjection, _eyePosition, _materialOverride);

    jack_racket_cube.material.color = glm::vec3(1.000f, 0.894f, 0.769f); // skin colour

    // forearm (skin)
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 5.0f, 1.0f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.2f, 1.0f));

    // arm (skin)
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-45.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 4.0f, 1.0f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(1.0f, 0.25f, 1.0f));

    // racket //
    jack_racket_cube.material.color = glm::vec3(0.0f, 0.5f, 0.5f); // colour

    // racket handle
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 5.0f, 0.5f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.20f, 2.0f));

    // base
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, -2.5f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 5.0f, 0.5f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, 0.20f, 2.0f));

    // left side
    // world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 7.0f, 0.5f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, (1.0f / 7.0f), 2.0f));

    // top side
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 7.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 5.0f, 0.5f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, (1.0f / 5.0f), 2.0f));

    // Right side
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 5.0f, 0.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.5f, 7.0f, 0.5f));
    jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix,
                                    racket_render_mode, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(2.0f, (1.0f / 7.0f), 2.0f));

    // setup for nets
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.2f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 4.5f, 0.1f));

    jack_racket_cube.material.color = glm::vec3(1.0f, 1.0f, 1.0f); // colour

    //||||||||||||||||||||||||
    float j;
    for (j = 0; j < 6.5; j += 0.5)
    {

        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(10.0f, 1.0f / 4.5f, 10.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 4.5f, 0.1f));
        jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride);
    }

    //-------------------------
    // setup
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(10.0f, 1.0f / 4.5f, 10.0f));
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(-90.0f, 0.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.25f, 0.25f));

    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 6.5f, 0.1f));

    for (j = 0; j < 4.5; j += 0.5)
    {
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(10.0f, 1.0f / 6.5f, 10.0f));
        world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.5f));
        world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(0.1f, 6.5f, 0.1f));
        jack_racket_cube.DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, racket_render_mode, _materialOverride);
    }
}

// augusto letter A
void Renderer::DrawOneA(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    auto scale_factor = glm::vec3(0.75f, 0.75f, 0.75f); // scale for one cube

    // base transform
    // rotating first, because we want to keep the original center of rotation
    world_transform_matrix = Transforms::RotateDegrees(world_transform_matrix, glm::vec3(0.0f, 90.0f, 0.0f));
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-scale_factor.x * 1.5f, 20.0f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, glm::vec3(scale_factor));

    // long left A vertical cubes
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    // short top A horizontal cubes
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(1.0f, 0.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    // long right A vertical cubes
    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(1.0f, -1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    // short middle A horizontal cubes

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.0f, 2.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
    letter_cubes[0].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
}

// gabrielle letter G
void Renderer::DrawOneG(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    auto scale_factor = glm::vec3(0.75f, 0.75f, 0.75f);               // scale for one cube
    letter_cubes[1].material.color = glm::vec3(1.0f, 0.714f, 0.757f); // white net colour

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 20.0f, -3.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, -0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 2 * -0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, -0.75f, -0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[1].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

// jack letter J
void Renderer::DrawOneJ(glm::mat4 world_transform_matrix, const glm::mat4& _viewProjection,const glm::vec3& _eyePosition, const Shader::Material *_materialOverride)
{
    auto scale_factor = glm::vec3(0.75f, 0.75f, 0.75f);               // scale for one cube
    letter_cubes[2].material.color = glm::vec3(1.0f, 0.714f, 0.757f); // white net colour

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 23.0f, -3.0f)); // go up and center

    // since we want it to be blocky, gotta do it like pixel art
    // one block at a time
    // hence same scaling and jump of position

    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.75f)); // 1 block to the left
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -1.5f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.75f, 0.0f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, 0.75f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);

    world_transform_matrix = glm::translate(world_transform_matrix, glm::vec3(0.0f, 0.0f, -1.50f));
    world_transform_matrix = glm::scale(world_transform_matrix, scale_factor);
    letter_cubes[2].DrawFromMatrix(_viewProjection, _eyePosition, world_transform_matrix, GL_TRIANGLES, _materialOverride);
    world_transform_matrix = glm::scale(world_transform_matrix, 1.0f / scale_factor);
}

void Renderer::ResizeCallback(GLFWwindow *_window, int _displayWidth, int _displayHeight)
{
    viewport_width = _displayWidth;
    viewport_height = _displayHeight;

    main_camera->SetViewportSize((float)viewport_width, (float)viewport_height);
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
