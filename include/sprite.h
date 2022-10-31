#pragma once

#include <global.h>
#include <vector>

struct Sprite
{
    vec2 pos;
    vec2 size;
    vec2 uv;

    Texture* tex;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int indices_count;

    Sprite() {}
    Sprite(const vec2& _pos, const vec2& _size, const vec2& _uv, Texture* _tex);

    void build();
    void draw();
    void destroy();
};