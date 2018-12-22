#include <framework.h>
#include <linalg.h>

#include <Nraster/buffer.h>
#include <Nraster/render.h>
#include <Nraster/shader.h>

const NRfloat near = 0.5f;
const NRfloat far = 5.0f;
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

std::vector<NRfloat> vertecies4d = 
{
    0.5f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f 
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
    window = glfwCreateWindow(640, 480, "cube4d", NULL, NULL);
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

    auto vao = nr::RenderData(nr::Primitive::POINTS);
    
    /*float rotate1[16];
    rotation(4, {0, 3}, M_PI / 4, rotate1);
    float rotate2[16];
    rotation(4, {1, 2}, -M_PI / 7, rotate2);
    float rotate[16];
    matmul(4, 4, 4, rotate1, rotate2, rotate);

    float rotated[16 * 4];*/
    
    for (unsigned i = 0; i < 16 * 4; i += 4)
    {
        //matmul(4, 4, 1, rotate, vertecies4d.data() + i, rotated + i);
        vertecies4d[i]   -= 1;
        vertecies4d[i+1] -= 1;
        vertecies4d[i+2] -= 1;
        vertecies4d[i+3] -= 1;
    }

    auto vertecies = std::vector<NRfloat>();

    for (unsigned int i = 0; i < 16 * 4; i += 4)
    {
        vertecies.push_back(vertecies4d[i]     / abs(vertecies4d[i + 3]));
        vertecies.push_back(vertecies4d[i + 1] / abs(vertecies4d[i + 3]));
        vertecies.push_back(vertecies4d[i + 2] / abs(vertecies4d[i + 3]));
    }

    printf("\n");
    for (unsigned int i = 0; i < vertecies.size(); i += 3)
    {
        printf("%f, %f, %f\n", vertecies[i], vertecies[i+1], vertecies[i+2]);
    }

    auto vertex_buffer = nr::Buffer(vertecies, 3u);
    vao.bindVertexBuffer(&vertex_buffer);

    auto program = nr::Render();
    
    auto vertex_shader = nr::Shader(loadFile("shaders/multiD.vert"), true, nr::Role::VERTEX, nrErr);
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

