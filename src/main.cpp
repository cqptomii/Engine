#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <../src/engine/utils.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // On ferme la page
}

const char* vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.8, 0.3, 0.2, 1.0);
}
)";

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE)
#endif


    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    std::vector<float> vertices = {
        0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    std::vector<unsigned int> indices = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = makeVBO<float>(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    unsigned int EBO = makeVBO<unsigned int>(indices, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Shader processing
    unsigned int shaderProgram = makeShaderProgram(vertexShaderSource, fragmentShaderSource);

    while (!glfwWindowShouldClose(window))
    {

        process_input(window);

        // Modification de la couleur de fond
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw on the screen

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glfwSwapBuffers(window); // Update Window pixel buffer
        glfwPollEvents(); // Check if any event is triggered
    }

    return 0;
}