//default lit vertex shader

#version 330 core

uniform mat4 u_model_transform; //model matrix
uniform mat4 u_view_projection; //view projection matrix

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec3 vNormal; //vertex input normal

out vec3 Normal;
out vec3 FragPos;

void main() {
    gl_Position = u_view_projection * u_model_transform * vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader

    Normal = mat3(transpose(inverse(u_model_transform))) * vNormal; //we need to transform the normal with the normal matrix (https://learnopengl.com/Lighting/Basic-Lighting & http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/)
    FragPos = vec3(u_model_transform * vec4(vPos, 1.0));
}