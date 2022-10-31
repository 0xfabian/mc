#pragma once

#include <glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    int data;

    Vertex() {}

    Vertex(const glm::vec3& _pos, int normal, int light, int ao, const glm::ivec2& tex_coord, int uv)
    {
        int _data = light & 0xff;

        _data += (normal & 7) << 8;
        _data += (ao & 3) << 11;
        _data += (uv & 3) << 13;
        _data += (tex_coord.x & 0x1f) << 15;
        _data += (tex_coord.y & 0x1f) << 20;

        pos = _pos;
        data = _data;
    }
};