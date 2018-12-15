#include <Nraster/predefs.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <vector>
#include <stdio.h>

#include <Nraster/buffer.h>
#include <Nraster/render.h>
#include <Nraster/shader.h>

#include "../inc/glutil.h"

void nrCheckError(const nr::Error& err, const char* file, const int line)
{
    if (nr::error::isFailure(err))
    {
        std::cerr << "at " << file << ":" << line << ": " << (NRint)err << std::endl;
        throw std::runtime_error("");
    }
}

#define nrCheckError(err) nrCheckError(err, __FILE__, __LINE__);

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

    auto nrErr = nr::Error::NO_ERROR;
        
    std::vector<NRfloat> vertecies = 
    {
        -0.6f, -0.4f,
        0.6f , -0.4f,
        0.0f ,  0.6f,
    };

    std::vector<NRfloat> colors = 
    {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f 
    };

    auto vao = nr::RenderData(nr::Primitive::TRIANGLES);
    
    auto vertex_buffer = nr::Buffer(vertecies, 2u);
    vao.bindBuffer(0u, &vertex_buffer);
    
    auto color_buffer  = nr::Buffer(colors, 3u);
    vao.bindBuffer(1u, &color_buffer);


    auto program = nr::Render();

    auto vertex_shader = nr::Shader(loadFile("shaders/shader.vert"), true, nr::ShaderType::VERTEX, nrErr);
    nrCheckError(nrErr);
    nrCheckError(program.bindShader(&vertex_shader));

    auto fragment_shader = nr::Shader(loadFile("shaders/shader.frag"), true, nr::ShaderType::FRAGMENT, nrErr);
    nrCheckError(nrErr);
    nrCheckError(program.bindShader(&fragment_shader));    

    nrCheckError(program.link());

    nrCheckError(vao.finalizeBindings());    
    nr::Render::setClearColor(0.0, 0.0, 0.0, 0.0);
    
    NRint width, height;
    while (!glfwWindowShouldClose(window))
    {   
        glfwGetFramebufferSize(window, &width, &height);

        nr::Render::viewPort(0, 0, width, height);

        nr::Render::clearColor();
        
        nrCheckError(program.drawArrays(vao));
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
