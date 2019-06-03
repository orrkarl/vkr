#include <utils.h>
#include <linalg.h>

#include <stdio.h>
#include <iostream>
#include <chrono>

#include <cmath>

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

NRfloat h_near[]
{
    -5, -5, 0.5, 0.5
};

NRfloat h_far[]
{
    5, 5, 10, 10
};

void transform(Triangle4d triangles[48 * 4], const NRuint tick)
{
    auto angle = tick * M_PI / 20;
    printf("Transform angle: %f\n", angle * 180 / M_PI);
    Matrix r = Matrix::rotation(X, W, angle);
    Matrix t = Matrix::translation(1.5, 1.5, 2, 1);
    
    Matrix op = t * r;

    Vector4d cube[16];
    for (auto i = 0; i < 16; ++i)
    {
        (op * h_cube[i]).toVector4d(cube + i);
    }

    reduce4Cube(cube, triangles);
}

int main(const int argc, const char* argv[])
{
    GLFWwindow* wnd;
    cl_int cl_err = CL_SUCCESS;

    nr::ScreenDimension screenDim = { 640, 480 };
    const NRuint dim = 4;
    nr::__internal::BinQueueConfig config = { 32, 32, 256 };
    
    cl::CommandQueue q = cl::CommandQueue::getDefault();

    std::unique_ptr<GLubyte> bitmap(new GLubyte[3 * screenDim.width * screenDim.height]);
    
    Triangle4d h_triangles[48 * 4]; 

    if (!init("Nraster Demo 4d", screenDim, wnd)) return EXIT_FAILURE;

    glViewport(0, 0, screenDim.width, screenDim.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);    

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

    if (nr::error::isFailure(cl_err = pipeline.setup(dim, 48 * 4, (NRfloat*) h_triangles, h_near, h_far, screenDim, config, 1, frame)))
    {
        std::cout << "Failed to setup pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }

    auto t0 = std::chrono::system_clock::now();
    transform(h_triangles, std::stoi(argv[argc - 1]));
    q.enqueueWriteBuffer(pipeline.vertexShader.params.points.getBuffer(), CL_FALSE, 0, sizeof(h_triangles), h_triangles);
    q.enqueueFillBuffer(pipeline.binRasterizer.params.binQueues.getBuffer(), 0.0f, 0, pipeline.binRasterizer.params.binQueues.getBuffer().getInfo<CL_MEM_SIZE>());
    q.enqueueFillBuffer(pipeline.fineRasterizer.params.frameBuffer.color.getBuffer(), (uint8_t) 0, 0, pipeline.fineRasterizer.params.frameBuffer.color.getBuffer().getInfo<CL_MEM_SIZE>());
    q.enqueueFillBuffer(pipeline.fineRasterizer.params.frameBuffer.depth.getBuffer(), 0.0f, 0, pipeline.fineRasterizer.params.frameBuffer.depth.getBuffer().getInfo<CL_MEM_SIZE>());
    q.finish();
    pipeline(q);
    q.enqueueReadBuffer(frame.color.getBuffer(), CL_TRUE, 0, 3 * screenDim.width * screenDim.height * sizeof(GLubyte), bitmap.get());
    q.finish();
    glDrawPixels(640, 480, GL_RGB, GL_UNSIGNED_BYTE, bitmap.get());
    glfwSwapBuffers(wnd);
    auto t1 = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = t1 - t0;
    std::cout << "Elapsed: " << diff.count() * 1000 << "ms" << std::endl; 

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}