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

void setupCube(Triangle4d* triangles)
{
    Vector4d cube[16];
    
    Matrix r = Matrix::rotation(X, Y, M_PI / 4);
    Matrix t = Matrix::translation(0, 0, 5, 1);
    Matrix s = Matrix::scale(0.5);
    Matrix ops = t * s * r;

    for (auto i = 0; i < 16; ++i)
    {
        h_cube[i] = ops * h_cube[i];
        h_cube[i].toVector4d(cube + i);
    }

    reduce4Cube(cube, triangles);
}

int main()
{
    GLFWwindow* wnd;
    cl_int cl_err = CL_SUCCESS;

    nr::ScreenDimension screenDim = { 640, 480 };
    const NRuint dim = 4;
    nr::__internal::BinQueueConfig config = { 32, 32, 256 };
    
    cl::CommandQueue q = cl::CommandQueue::getDefault();
    
    std::unique_ptr<GLubyte> bitmap(new GLubyte[3 * screenDim.width * screenDim.height]);
    
    Triangle4d h_triangles[48 * 4]; 
    setupCube(h_triangles);

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

    auto start = std::chrono::system_clock::now();
    if ((cl_err = pipeline(q)) != CL_SUCCESS)
    {
        std::cout << "Failed to execute pipeline: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")\n";
        return EXIT_FAILURE;
    }
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> delta = end - start;

    std::cout << "Elapsed: " << delta.count() * 1000 << "ms" << std::endl;

    printf("Reading from buffer...\n");
    q.enqueueReadBuffer(frame.color.getBuffer(), CL_TRUE, 0, 3 * screenDim.width * screenDim.height * sizeof(GLubyte), bitmap.get());
    q.finish();
    
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