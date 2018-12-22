#pragma once

#include <Nraster/predefs.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

std::string loadFile(const std::string& fileName)
{
    std::ifstream f(fileName);

    if (!f)
    {
        std::cerr << "could not find\\open file " << fileName << std::endl;
        throw std::exception();
    }

    std::stringstream buffer;
    
    buffer << f.rdbuf();
    return buffer.str();
}

void validate(const nr::Error& err, const char* file, const int line)
{
    if (nr::error::isFailure(err))
    {
        fprintf(stderr, "at %s:%d: %d\n", file, line, (NRint) err);
        std::exit(EXIT_FAILURE);
    }
}

#define validate(err) validate(err, __FILE__, __LINE__);

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

#define RED3    (1.0f, 0.0f, 0.0f)
#define GREEN3  (0.0f, 1.0f, 0.0f)
#define BLUE3   (0.0f, 0.0f, 1.0f)



