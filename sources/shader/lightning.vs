#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec Normal;

void main() {
    gl_Position = projection * view * model * vec4(aPos,)
    Normal = aNormal;
}