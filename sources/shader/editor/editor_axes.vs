#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;


struct InstanceData{
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(std430, binding = 0) buffer TransformData{
    InstanceData data[];
};


out vec3 vColor;

void main()
{
    int i = gl_InstanceID;

    vColor = aColor;
    gl_Position = data[i].projection * data[i].view * data[i].model * vec4(aPos, 1.0);
}
