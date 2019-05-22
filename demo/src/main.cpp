#include <framework.h>

#include <stdio.h>
#include <iostream>

NRfloat h_triangle[9]
{
    -5, -2.5, 3,
    5, -2.5, 2,
    0, 5, 2.5
};

NRfloat h_near[3]
{
    -3, -3, 0.5
};

NRfloat h_far[3]
{
    3, 3, 10
};

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
    GLFWwindow* wnd;
    cl_int cl_err = CL_SUCCESS;

    nr::ScreenDimension screenDim = { 640, 480 };
    const NRuint dim = 3;
    nr::__internal::BinQueueConfig config = { 32, 32, 10 };
    cl::CommandQueue q = cl::CommandQueue::getDefault();

    if (!init("Nraster Demo 3d", screenDim, error_callback, key_callback, wnd)) return EXIT_FAILURE;

    nr::__internal::Module nr_code = mkFullModule(dim, &cl_err);
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create nr_code: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }

    nr::FrameBuffer frame = mkFrameBuffer(screenDim, &cl_err);
    if (nr::error::isFailure(cl_err))
    {
        std::cout << "Could not create frame buffer: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }

    auto pipeline = FullPipeline(nr_code, &cl_err);
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }

    if (nr::error::isFailure(cl_err = pipeline.setup(dim, 1, h_triangle, h_near, h_far, screenDim, config, 1, frame)))
    {
        std::cout << "Failed to setup pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }

    if ((cl_err = pipeline(q)) != CL_SUCCESS)
    {
        std::cout << "Failed to execute pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }
    
    if ((cl_err = q.finish()) != CL_SUCCESS)
    {
        std::cout << "Failed to execute pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }
    
    pipeline.writeToGL();
    glfwSwapBuffers(wnd);

    while (!glfwWindowShouldClose(wnd))
    {
    }
}