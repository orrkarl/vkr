#include <utils.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <stdio.h>

#include <utils/converters.h>

nr_float h_triangle[9]
{
    -5, -2.5, 3,
    0, 5, 2.5,
    5, -2.5, 2
};

nr_float h_near[3]
{
    -3, -3, 0.5
};

nr_float h_far[3]
{
    3, 3, 10
};

int main()
{
    GLFWwindow* wnd;
    cl_int cl_err = CL_SUCCESS;

    nr::ScreenDimension screenDim = { 640, 480 };
    const nr_uint dim = 3;
    nr::__internal::BinQueueConfig config = { 32, 32, 5 };
    nr::CommandQueue q = nr::CommandQueue::getDefault();
    std::unique_ptr<nr::RawColorRGB> bitmap(new nr::RawColorRGB[screenDim.width * screenDim.height]);

    if (!init("Nraster Demo 3d", screenDim, wnd)) return EXIT_FAILURE;

    // Prepare to write to framebuffer
    glViewport(0, 0, screenDim.width, screenDim.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);    

    nr::Module nr_code = mkFullModule(dim, &cl_err);
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

    auto start = std::chrono::system_clock::now();
    
    // Execute the pipeline on the input
    if ((cl_err = pipeline(q)) != CL_SUCCESS)
    {
        std::cout << "Failed to execute pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> delta = end - start;

    std::cout << "Elapsed: " << delta.count() * 1000 << "ms" << std::endl;

    printf("Reading from buffer...\n");
    q.enqueueBufferReadCommand(frame.color, true, screenDim.width * screenDim.height, bitmap.get());
    
    // Write directly to back-buffer
    printf("Drawing pixels...\n");
    glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, bitmap.get());
    
    printf("Swapping buffers...\n");
    glfwSwapBuffers(wnd);

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}