#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 vCol;

out vec3 color;

void main()
{
    gl_Position = vPos;
    color = vCol;
}