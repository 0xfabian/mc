#include <raycast.h>

// http://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game#comment160436_49423

float intbound(float s, float ds)
{
    if (ds < 0 && roundf(s) == s)
        return 0;

    if (ds > 0)
        return (ceilf(s) - s) / fabsf(ds);

    return (s - floorf(s)) / fabsf(ds);
}

vec3 vec3_intbound(const vec3& pos, const vec3& dir)
{
    return vec3(intbound(pos.x, dir.x), intbound(pos.y, dir.y), intbound(pos.z, dir.z));
}

HitInfo raycast(const vec3& origin, const vec3& dir, float max_distance, Chunk* chunk)
{
    HitInfo ret;
    ret.hit = false;

    if (chunk == NULL)
        return ret;

    vec3 rel_origin = origin - chunk_pos_to_world(chunk->pos);
    ivec3 pos = ivec3(rel_origin.x, floorf(rel_origin.y), rel_origin.z);
    ivec3 step = sign(dir);

    vec3 t = vec3_intbound(rel_origin, dir);
    vec3 t_unit = (vec3)step / dir;

    float distance = 0.0f;
    ivec3 face;

    while (distance < max_distance)
    {
        if (t.x < t.y)
        {
            if (t.x < t.z)
            {
                pos.x += step.x;
                distance = t.x;
                t.x += t_unit.x;
                face = ivec3(-step.x, 0, 0);
            }
            else
            {
                pos.z += step.z;
                distance = t.z;
                t.z += t_unit.z;
                face = ivec3(0, 0, -step.z);
            }
        }
        else
        {
            if (t.y < t.z)
            {
                pos.y += step.y;
                distance = t.y;
                t.y += t_unit.y;
                face = ivec3(0, -step.y, 0);
            }
            else
            {
                pos.z += step.z;
                distance = t.z;
                t.z += t_unit.z;
                face = ivec3(0, 0, -step.z);
            }
        }

        if (pos.y >= 0 && pos.y < CHUNK_HEIGHT)
        {
            chunk = get_neighbor_chunk(chunk, pos);

            if (chunk == NULL)
                return ret;

            if (chunk->get(pos)->id != BlockID::AIR)
            {
                ret.hit = true;
                ret.chunk = chunk;
                ret.block_pos = pos;
                ret.face_normal = face;
                ret.hit_point = origin + distance * dir;

                return ret;
            }
        }
    }

    return ret;
}