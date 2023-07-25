//default lit fragment shader

#version 330 core

uniform vec3 u_cam_pos; //cam position

uniform vec3 u_light_pos; //main light position
uniform vec3 u_light_color; //main light color

uniform float u_ambient_strength; //ambient light strength
uniform float u_specular_strength; //specular light strength
uniform int u_shininess; //light shininess

uniform vec3 u_color; //cube color
uniform float u_alpha; //cube opacity

in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor; //rgba color output

//entrypoint
void main() {
    float light_strength = 20;

    //ambient lighting calculation
    vec3 ambient = u_ambient_strength * u_light_color;

    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_light_pos - FragPos);
    float lightDistance = length(u_light_pos - FragPos);

    float diffFactor = dot(lightDir, norm);
    vec3 diffuse = diffFactor * u_light_color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * u_specular_strength * u_light_color;

    vec3 colorResult = u_color * (ambient + (diffuse + specular) * light_strength * 0.883 / (0.18 + 0.0 * lightDistance + 0.51 * lightDistance * lightDistance));

    fragColor = vec4(colorResult, u_alpha);
}