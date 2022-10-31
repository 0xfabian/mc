#pragma once

#include <glad/glad.h>

struct Texture
{
    GLuint id;
    int unit;

    int width;
    int height;
    int chan_num;

    Texture() {}
    Texture(const char* path, int _unit);

    void bind();
    void use_mipmap(const char* path, int level);
    void destroy();
};