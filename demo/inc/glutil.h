#include <GL/glew.h>

#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <exception>

#pragma once

void CheckError(const char* msg, const int line, const char* file)
{
    GLenum err;
    bool failed = false;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "Error in file '" << file << "' at line " << line << ": " << msg << " (" << err << ")" << std::endl;
        failed = true;
    }
    if (failed) throw std::runtime_error("gl error detected");
}

#define CheckError(s) CheckError(s, __LINE__, __FILE__)

GLint getProgramField(GLuint program, GLenum field) 
{
    GLint ret;
    glGetProgramiv(program, field, &ret);
    return ret;
}

GLuint loadShader(GLenum shaderType, const std::string& shaderFileName)
{
    std::ifstream f(shaderFileName);

    if (!f)
    {
        std::cerr << "could not find\\open file " << shaderFileName << std::endl;
        throw std::exception();
    }

    std::stringstream buffer;
    
    buffer << f.rdbuf();

    auto ret = glCreateShader(shaderType);
    auto mem = buffer.str();
    auto mem_base = mem.c_str();
    glShaderSource(ret, 1, &mem_base, NULL);
    CheckError("could not use shader source");
    glCompileShader(ret);
    CheckError("could not compile shader");

    GLint status = 0;
    glGetShaderiv(ret, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint len = 0;
        glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &len);
        auto log = std::vector<GLchar>(len);
        auto base = &log[0];
        glGetShaderInfoLog(ret, log.size(), &len, base);
        glDeleteShader(ret);
        
        std::cerr << "when compiling " << shaderFileName << ":" << std::endl;
        throw std::runtime_error(base);
    }

    return ret;
}

void validLinkProgram(GLuint program)
{
    glLinkProgram(program);
    CheckError("could not link program");
    if (getProgramField(program, GL_LINK_STATUS) == GL_FALSE)
    {
        std::cerr << "error while linking program:" << std::endl;
        auto len = getProgramField(program, GL_INFO_LOG_LENGTH);
        if (len)
        {
            GLsizei actualLength;
            auto log = std::vector<GLchar>(len);
            auto base = &log[0];
            glGetProgramInfoLog(program, len, &actualLength, base);
            std::cerr << base << std::endl;
        }
        throw std::runtime_error("could not link program"); 
    }
}
