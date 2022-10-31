#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 cam_mat;

void main()
{
    gl_Position = cam_mat * vec4(pos, 1.0f);
}