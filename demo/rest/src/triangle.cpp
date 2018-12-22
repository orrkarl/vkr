#include <framework.h>

#include <Nraster/buffer.h>
#include <Nraster/render.h>
#include <Nraster/shader.h>

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "triangle", NULL, NULL);
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
    vao.bindVertexBuffer(&vertex_buffer);
    
    auto color_buffer  = nr::Buffer(colors, 3u);
    vao.bindAttribute(1u, &color_buffer);
    validate(nr::utils::getLastGLError());
    auto program = nr::Render();
    validate(nr::utils::getLastGLError());

    auto vertex_shader = nr::Shader(loadFile("shaders/basic2d.vert"), true, nr::Role::VERTEX, nrErr);
    if (nr::error::isFailure(nrErr))
    {
        std::cerr << vertex_shader.getCompilerLog(nrErr) << std::endl;
        throw std::runtime_error("could not compile shader");
    }
    validate(program.bindShader(&vertex_shader));

    auto fragment_shader = nr::Shader(loadFile("shaders/basic.frag"), true, nr::Role::FRAGMENT, nrErr);
    if (nr::error::isFailure(nrErr))
    {
        std::cerr << vertex_shader.getCompilerLog(nrErr) << std::endl;
        throw std::runtime_error("could not compile shader");
    }
    validate(program.bindShader(&fragment_shader));    

    validate(program.link());

    nr::Render::setClearColor(0.0, 0.0, 0.0, 0.0);
    
    NRint width, height;
    while (!glfwWindowShouldClose(window))
    {   
        glfwGetFramebufferSize(window, &width, &height);

        nr::Render::viewPort(0, 0, width, height);

        nr::Render::clearColor();
        
        validate(program.drawArrays(vao));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
