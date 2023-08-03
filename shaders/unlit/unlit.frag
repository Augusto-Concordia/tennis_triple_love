//default unlit fragment shader

#version 330 core

uniform vec3 u_color; //grid color
uniform float u_alpha; //grid opacity

uniform sampler2D u_texture;
uniform float u_texture_influence;

in vec2 FragUv;

layout(location = 0) out vec4 out_color; //rgba color output

//entrypoint
void main() {
    out_color = mix(vec4(u_color, u_alpha), texture(u_texture, FragUv), u_texture_influence) ;
}