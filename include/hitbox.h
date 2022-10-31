#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <global.h>

using namespace glm;

struct Hitbox
{
    dvec3 pos;
    dvec3 size;

    Hitbox() {}
    Hitbox(const dvec3& _pos, const dvec3& _size);

    void draw();
};

Hitbox get_bounds(const Hitbox& a, const Hitbox& b);
bool intersect(const Hitbox& a, const Hitbox& b);