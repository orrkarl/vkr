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
    nr::Error nr_err = nr::Error::NO_ERROR;

    if (!init("Nraster Demo 3d", error_callback, key_callback, wnd)) return EXIT_FAILURE;

    nr::__internal::Module nr_code(
        {nr::__internal::clcode::base,
         nr::__internal::clcode::bin_rasterizer,
         nr::__internal::clcode::fine_rasterizer,
         nr::__internal::clcode::vertex_shading},
         "-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3",
         &cl_err);
    
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create nr_code: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")";
        return EXIT_FAILURE;
    }

    // Creating Kernels
    auto vertex_shader = nr_code.makeKernel<nr::__internal::VertexShadingParams>("shade_vertex", &cl_err);
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create vertex shader kernel: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")";
        return EXIT_FAILURE;
    }

    auto bin_rasterizer = nr_code.makeKernel<nr::__internal::BinRasterizerParams>("bin_rasterize", &cl_err);
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create bin rasterizer kernel: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")";
        return EXIT_FAILURE;
    }

    auto fine_rasterizer = nr_code.makeKernel<nr::__internal::FineRasterizerParams>("fine_rasterize", &cl_err);
    if (cl_err != CL_SUCCESS)
    {
        std::cout << "Could not create fine rasterizer kernel: " << nr::utils::stringFromCLError(cl_err) << "(" << cl_err << ")";
        return EXIT_FAILURE;
    }


    while (!glfwWindowShouldClose(wnd))
    {

    }
}