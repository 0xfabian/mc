#version 460 core

layout (location = 0) in vec2 p;
layout (location = 1) in vec2 in_uv;

uniform int win_width;
uniform int win_height;
uniform vec3 pos;

out vec2 uv;

void main()
{
    vec2 pn = vec2((p.x + pos.x) / float(win_width), (p.y + pos.y) / float(win_height)) * 2.0;

    gl_Position = vec4(pn, 0.0, 1.0);

    uv = in_uv;
}