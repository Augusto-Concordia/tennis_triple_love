//default shadow mapper fragment shader

#version 330 core

uniform vec3 u_light_pos; //main light position

in vec3 FragPos;

layout(location = 0) out vec4 out_color; //rgba color output

//entrypoint
void main() {
    vec3 colorResult = vec3(length(u_light_pos - FragPos) / 10);

    out_color = vec4(colorResult, 1.0);
}