#version 460 core

layout(location = 0) in vec3 aPos;

out vec2 vNdc;

void main()
{
    // Full-screen quad vertices expected in NDC space [-1, 1].
    vNdc = aPos.xy;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
