#version 450

layout(location = 0) in vec3 vPos;

layout(location = 2) uniform mat4 perspective;
layout(location = 3) uniform vec2 offset;

out vec3 color;

void main()
{
    gl_Position = perspective * vec4(vPos.x + offset.x, vPos.y + offset.y, vPos.z, 1.0f);
    color = vec3(1.0, 0.0, 0.0);
}