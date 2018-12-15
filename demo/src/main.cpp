#include <Nraster/predefs.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <Nraster/buffer.h>
#include <Nraster/render.h>

#include "../inc/glutil.h"

static const struct
{
    float x, y;
    float r, g, b;
}

vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
int main(void)
{
    GLFWwindow* window;
    GLuint vao, vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true; // I HATE THIS!!!!!!
    auto err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << glewGetErrorString(err) << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    vertex_shader = loadShader(GL_VERTEX_SHADER, "shaders/shader.vert");
    fragment_shader = loadShader(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    program = glCreateProgram();
    CheckError("could not create program");
    
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    CheckError("could not attach shaders");
    validLinkProgram(program);
    
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    CheckError("could not find attributes");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    CheckError("at pos attirb assignging");
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
    CheckError("at col attirb assignging");
    
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        CheckError("at glViewport");
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CheckError("at Color reseting");
    
        glUseProgram(program);
        CheckError("at program use");

        glDrawArrays(GL_TRIANGLES, 0, 3);
        CheckError("at glDrawArrays");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
