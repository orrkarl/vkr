#include <framework.h>

#include <stdio.h>
#include <iostream>

NRfloat h_triangle[9]
{
    -5, -2.5, 3,
    0, 5, 2.5,
    5, -2.5, 2
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
    nr::__internal::BinQueueConfig config = { 32, 32, 5 };
    cl::CommandQueue q = cl::CommandQueue::getDefault();
    std::unique_ptr<GLubyte> bitmap(new GLubyte[3 * screenDim.width * screenDim.height]);

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

    // printf("Reading from buffer...\n");
    q.enqueueReadBuffer(frame.color.getBuffer(), CL_TRUE, 0, 3 * screenDim.width * screenDim.height * sizeof(GLubyte), bitmap.get());
    q.finish();
    
    // for (auto i = 0; i < screenDim.width * screenDim.height; ++i)
    // {
        // if (bitmap.get()[3 * i])
        // {
            // printf("Buffer not clear at idx %d: (%d, %d, %d)\n", i, bitmap.get()[3 * i], bitmap.get()[3 * i + 1], bitmap.get()[3 * i + 2]);
        // }
    // }

    // std::unique_ptr<NRuint> bins(new NRuint[pipeline.binRasterizer.params.binQueues.getBuffer().getInfo<CL_MEM_SIZE>() / sizeof(cl_uint)]);
    // q.enqueueReadBuffer(pipeline.binRasterizer.params.binQueues.getBuffer(), CL_TRUE, 0, pipeline.binRasterizer.params.binQueues.getBuffer().getInfo<CL_MEM_SIZE>(), bins.get());
    // q.finish();
// 
    // for (auto y = 0; y < 2; ++y)
    // {
        // for (auto x = 0; x < 2; ++x)
        // {
            // printf("Bin (%d, %d): ", x, y);
            // NRuint* currentQueue = bins.get() + (y * 2 + x) * (config.queueSize + 1); 
            // for (auto i = 0; i < config.queueSize + 1; ++i)
            // {
                // printf("%d ", currentQueue[i]);
            // }
            // printf("\n");
        // }
    // }

    GLboolean b;
    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &b);

    std::cout << "Result: " << (bool) b << std::endl;

    printf("Drawing pixels...\n");
    glViewport(0, 0, screenDim.width, screenDim.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    printf("pre gl error: %x\n", glGetError());
    glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, bitmap.get());
    printf("post gl error: %x\n", glGetError());
    printf("Swapping buffers...\n");
    glfwSwapBuffers(wnd);

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}