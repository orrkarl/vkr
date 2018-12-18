#version 450

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) uniform mat2 rotate;

out vec3 color;

void main()
{
    gl_Position = vec4(rotate * vPos, 0.0, 1.0);
    color = vCol;
}