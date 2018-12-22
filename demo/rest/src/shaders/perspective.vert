#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec3 vCol;

layout(location = 2) uniform mat4 perspective;
layout(location = 3) uniform vec2 offset;

out vec3 color;

void main()
{
    gl_Position = perspective * vec4(vPos.x + offset.x, vPos.y + offset.y, vPos.z, vPos.w);
    color = vCol;
}