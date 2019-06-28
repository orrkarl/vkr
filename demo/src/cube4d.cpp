#include <utils/converters.h>

#include <utils.h>
#include <linalg.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <thread>

Vector h_cube[]
{
    Vector(-1, -1, -1, -1, 1),
    Vector(-1, -1, -1,  1, 1),
    Vector(-1, -1,  1, -1, 1),
    Vector(-1, -1,  1,  1, 1),
    Vector(-1,  1, -1, -1, 1),
    Vector(-1,  1, -1,  1, 1),
    Vector(-1,  1,  1, -1, 1),
    Vector(-1,  1,  1,  1, 1),
    Vector( 1, -1, -1, -1, 1),
    Vector( 1, -1, -1,  1, 1),
    Vector( 1, -1,  1, -1, 1),
    Vector( 1, -1,  1,  1, 1),
    Vector( 1,  1, -1, -1, 1),
    Vector( 1,  1, -1,  1, 1),
    Vector( 1,  1,  1, -1, 1),
    Vector( 1,  1,  1,  1, 1),
};

nr_float h_near[]
{
    -5, -5, 0.5, 0.5
};

nr_float h_far[]
{
    5, 5, 10, 10
};

void transform(Triangle4d triangles[48 * 4], const nr_uint tick)
{
    auto angle = tick * M_PI / 20;

    Matrix r = Matrix::rotation(Y, Z, angle);
    Matrix t = Matrix::translation(1.5, 1.5, 2, 2);
    
    Matrix op = t * r;

    Vector4d cube[16];
    for (auto i = 0; i < 16; ++i)
    {
        Vector tmp = op * h_cube[i];
        tmp.toVector4d(cube + i);
    }

    reduce4Cube(cube, triangles);
}

void profilePipeline(
    const std::chrono::system_clock::time_point& t0,
    const std::chrono::system_clock::time_point& transform,
    const std::chrono::system_clock::time_point& buffers,
    const std::chrono::system_clock::time_point& pipeline,
    const std::chrono::system_clock::time_point& framebufferRead,
    const std::chrono::system_clock::time_point& framebufferWrite,
    const std::chrono::system_clock::time_point& bufferSwap)
{
    std::chrono::duration<nr_double> dt_transform = transform - t0;
    std::chrono::duration<nr_double> dt_buffers = buffers - transform;
    std::chrono::duration<nr_double> dt_pipeline = pipeline - buffers;
    std::chrono::duration<nr_double> dt_frameBufferRead = framebufferRead - pipeline;
    std::chrono::duration<nr_double> dt_frameBufferWrite = framebufferWrite - framebufferRead;
    std::chrono::duration<nr_double> dt_bufferSwap = bufferSwap - framebufferWrite;

    std::printf("Pipeline profile:\n");
    std::printf("\ttotal transform time - %fms\n",          dt_transform.count()        * 1000);
    std::printf("\ttotal buffers setup time - %fms\n",      dt_buffers.count()          * 1000);
    std::printf("\ttotal pipeline time - %fms\n",           dt_pipeline.count()         * 1000);
    std::printf("\ttotal frame buffer read time - %fms\n",  dt_frameBufferRead.count()  * 1000);
    std::printf("\ttotal frame buffer write time - %fms\n", dt_frameBufferWrite.count() * 1000);
    std::printf("\ttotal buffer swap time - %fms\n",        dt_bufferSwap.count()       * 1000);
}

int main(const int argc, const char* argv[])
{
    GLFWwindow* wnd;
    cl_status cl_err = CL_SUCCESS;

    nr::ScreenDimension screenDim = { 640, 480 };
    const nr_uint dim = 4;
    const nr_uint triangleCount = 48 * 4;
    nr::__internal::BinQueueConfig config = { 32, 32, 256 };
    
    nr::CommandQueue q = nr::CommandQueue::getDefault();

    std::unique_ptr<nr::RawColorRGB> bitmap(new nr::RawColorRGB[screenDim.width * screenDim.height]);
    
    Triangle4d h_triangles[triangleCount]; 

    if (!init("Nraster Demo 4d", screenDim, wnd)) return EXIT_FAILURE;

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

    if (nr::error::isFailure(cl_err = pipeline.setup(dim, triangleCount, (nr_float*) h_triangles, h_near, h_far, screenDim, config, 1, frame)))
    {
        std::cout << "Failed to setup pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }    

    nr_uint tick = 0;
    while (!glfwWindowShouldClose(wnd))
    {
        auto t0 = std::chrono::system_clock::now();
        transform(h_triangles, tick++);
        auto t_transform = std::chrono::system_clock::now();
        q.enqueueBufferWriteCommand(pipeline.vertexShader.points, false, triangleCount, (nr_float*) h_triangles);
        q.enqueueBufferFillCommand(pipeline.binRasterizer.binQueues, 0u);
        q.enqueueBufferFillCommand(pipeline.fineRasterizer.frameBuffer.color, { 0, 0, 0 });
        q.enqueueBufferFillCommand(pipeline.fineRasterizer.frameBuffer.depth, 0.0f);
        q.await();
        auto t_buffers = std::chrono::system_clock::now();
        pipeline(q);
        q.await();
        auto t_pipeline = std::chrono::system_clock::now();
        q.enqueueBufferReadCommand(frame.color, true, screenDim.width * screenDim.height, bitmap.get());
        q.await();
        auto t_framebufferRead = std::chrono::system_clock::now();
        glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, bitmap.get());
        auto t_framebufferWrite = std::chrono::system_clock::now();
        glfwSwapBuffers(wnd);
        auto t_bufferSwap = std::chrono::system_clock::now();
    
        auto end = std::chrono::system_clock::now();
        std::chrono::milliseconds diff(400 - (end - t0).count() * 1000000);
        std::this_thread::sleep_for(diff);
        
        glfwPollEvents();
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}