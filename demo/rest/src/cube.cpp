#include <framework.h>

#include <Nraster/buffer.h>
#include <Nraster/render.h>
#include <Nraster/shader.h>

const NRfloat near = 0.5f;
const NRfloat far = 3.0f;
const NRfloat scale = 1.0f;


std::vector<NRfloat> perspectiveMatrix = 
{
    scale, 0.0f, 0.0f, 0.0f,
    0.0f, scale, 0.0f, 0.0f,
    0.0f, 0.0f, (far + near) / (near - far), (2 * far * near) / (near - far),
    0.0f, 0.0f, -1.0f, 0.0f
};

std::vector<NRfloat> camOffset = 
{
    0.8f, 0.5f
};

std::vector<NRfloat> vertecies = 
{
    0.25f,  0.25f, -1.25f, 1.0f,
    0.25f, -0.25f, -1.25f, 1.0f,
    -0.25f,  0.25f, -1.25f, 1.0f,

    0.25f, -0.25f, -1.25f, 1.0f,
    -0.25f, -0.25f, -1.25f, 1.0f,
    -0.25f,  0.25f, -1.25f, 1.0f,

    0.25f,  0.25f, -2.75f, 1.0f,
    -0.25f,  0.25f, -2.75f, 1.0f,
    0.25f, -0.25f, -2.75f, 1.0f,

    0.25f, -0.25f, -2.75f, 1.0f,
    -0.25f,  0.25f, -2.75f, 1.0f,
    -0.25f, -0.25f, -2.75f, 1.0f,

    -0.25f,  0.25f, -1.25f, 1.0f,
    -0.25f, -0.25f, -1.25f, 1.0f,
    -0.25f, -0.25f, -2.75f, 1.0f,

    -0.25f,  0.25f, -1.25f, 1.0f,
    -0.25f, -0.25f, -2.75f, 1.0f,
    -0.25f,  0.25f, -2.75f, 1.0f,

    0.25f,  0.25f, -1.25f, 1.0f,
    0.25f, -0.25f, -2.75f, 1.0f,
    0.25f, -0.25f, -1.25f, 1.0f,

    0.25f,  0.25f, -1.25f, 1.0f,
    0.25f,  0.25f, -2.75f, 1.0f,
    0.25f, -0.25f, -2.75f, 1.0f,

    0.25f,  0.25f, -2.75f, 1.0f,
    0.25f,  0.25f, -1.25f, 1.0f,
    -0.25f,  0.25f, -1.25f, 1.0f,

    0.25f,  0.25f, -2.75f, 1.0f,
    -0.25f,  0.25f, -1.25f, 1.0f,
    -0.25f,  0.25f, -2.75f, 1.0f,

    0.25f, -0.25f, -2.75f, 1.0f,
    -0.25f, -0.25f, -1.25f, 1.0f,
    0.25f, -0.25f, -1.25f, 1.0f,

    0.25f, -0.25f, -2.75f, 1.0f,
    -0.25f, -0.25f, -2.75f, 1.0f,
    -0.25f, -0.25f, -1.25f, 1.0f,
};

std::vector<NRfloat> colors = 
{
    0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.8f, 0.8f, 0.8f,
	0.8f, 0.8f, 0.8f,
	0.8f, 0.8f, 0.8f,

	0.8f, 0.8f, 0.8f,
	0.8f, 0.8f, 0.8f,
	0.8f, 0.8f, 0.8f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,

	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
};


int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "cube", NULL, NULL);
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


    auto vao = nr::RenderData(nr::Primitive::TRIANGLES);
    
    auto vertex_buffer = nr::Buffer(vertecies, 4u);
    vao.bindVertexBuffer(&vertex_buffer);
    
    auto color_buffer = nr::Buffer(colors, 3u);
    validate(vao.bindAttribute(1u, &color_buffer));


    auto program = nr::Render();
    
    auto vertex_shader = nr::Shader(loadFile("shaders/perspective.vert"), true, nr::Role::VERTEX, nrErr);
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

    auto perspective = nr::Uniform(perspectiveMatrix.data(), 4, 4);
    validate(program.bindUniform(2u, &perspective));

    auto offset = nr::Uniform(camOffset.data(), 1, 2);
    validate(program.bindUniform(3u, &offset));

    nr::Render::setClearColor(0.0, 0.0, 0.0, 0.0);

    nr::Render::cullFace();

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

