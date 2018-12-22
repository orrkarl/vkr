#version 450

layout(location = 0) in vec3 vPos;

layout(location = 2) uniform mat4 perspective;
layout(location = 3) uniform vec2 offset;
layout(location = 4) uniform float far;

out vec3 color;

const vec3 red = vec3(1.0, 0.0, 0.0);
const vec3 green = vec3(0.0, 1.0, 0.0);

void main()
{
    gl_Position = perspective * vec4(vPos.x + offset.x, vPos.y + offset.y, vPos.z, 1.0f);
    color = mix(red, green, abs(vPos.z) / far);
}