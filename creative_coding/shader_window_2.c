#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

// Vertex Shader source code
const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
    vTexCoord = aTexCoord;

    vec4 position = vec4(aPosition, 1.0);
    position.xy = position.xy * 2.0 - 1.0;

    gl_Position = position;
}
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
#version 330 core

precision highp float;

in vec2 vTexCoord;

uniform vec2 uTileSize;
uniform vec2 uScreenSize;

float getTilePattern(vec2 tilePos) {
    float pattern = 0.0;
    pattern += sin(tilePos.x * 10.0) * 0.5 + 0.5;
    pattern += cos(tilePos.y * 10.0) * 0.5 + 0.5;
    pattern *= 0.5;
    return pattern;
}

vec3 randomColor(float seed) {
    return vec3(fract(sin(seed * 43758.5453)) * 0.5 + 0.5,
                fract(cos(seed * 75430.3542)) * 0.5 + 0.5,
                fract(tan(seed * 14157.4731)) * 0.5 + 0.5);
}

void main() {
    vec2 fragmentCoord = vTexCoord * uScreenSize;
    float numCols = floor(uScreenSize.x / uTileSize.x);
    float numRows = floor(uScreenSize.y / uTileSize.y);
    float tileIndexX = floor(fragmentCoord.x / uTileSize.x);
    float tileIndexY = floor(fragmentCoord.y / uTileSize.y);
    vec2 tileCenter = (vec2(tileIndexX, tileIndexY) + 0.5) * uTileSize;
    float x = abs((fragmentCoord.x - tileCenter.x) / uScreenSize.x * 10.0);
    float y = abs((fragmentCoord.y - tileCenter.y) / uScreenSize.y * 10.0);
    float pattern = max(x, y);

    vec2 tileOffset = fragmentCoord - vec2(tileIndexX, tileIndexY) * uTileSize;

    float seed = tileIndexX + tileIndexY * numCols + tileOffset.x + tileOffset.y;

    pattern *= getTilePattern(vec2(tileIndexX, tileIndexY));
    vec3 fragmentColor = randomColor(seed);

    gl_FragColor = vec4(pattern * fragmentColor.x, pattern * fragmentColor.y, pattern, 1.0);
}
)";

// Function to compile shaders and create a shader program
GLuint compileShaders(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Initialize and set up a window
GLFWwindow* initializeWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    return window;
}

int main() {
    int width = 800;
    int height = 600;

    GLFWwindow* window = initializeWindow(width, height, "OpenGL Window");

    GLuint shaderProgram = compileShaders(vertexShaderSource, fragmentShaderSource);

    GLfloat vertices[] = {
        // Positions        // Texture Coords
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        GLint screenSizeLoc = glGetUniformLocation(shaderProgram, "uScreenSize");
        glUniform2f(screenSizeLoc, (float)width, (float)height);

        GLint tileSizeLoc = glGetUniformLocation(shaderProgram, "uTileSize");
        glUniform2f(tileSizeLoc, 50.0f, 50.0f); // Set the tile size

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
