#pragma once

#include <global.h>
#include <string>
#include <vector>

struct Text
{
    ivec2 pos;
    std::string str;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int indices_count;

    Text() {}
    Text(const ivec2& _pos);

    void set_str(const std::string& _str);
    void build();
    void draw();
    void destroy();
};