#pragma once

#include <math.h>
#include <chunk.h>

struct HitInfo
{
    bool hit;

    Chunk* chunk;
    ivec3 block_pos;
    ivec3 face_normal;

    vec3 hit_point;
};

HitInfo raycast(const vec3& origin, const vec3& dir, float max_distance, Chunk* chunk);