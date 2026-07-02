#version 460 core
layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform CameraData{
    mat4 view;
    mat4 projection;
};

layout(std140, binding = 1) uniform ModelData{
    mat4 model;
};


void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}