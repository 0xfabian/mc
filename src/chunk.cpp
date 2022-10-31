#include <chunk.h>

Chunk::Chunk(const ivec2& _pos)
{
    pos = _pos;

    north = NULL;
    south = NULL;
    east = NULL;
    west = NULL;

    dirty = false;
    can_draw = false;

    mesh = ChunkMesh(this);
}

BlockState* Chunk::get(const ivec3& p)
{
    return &blocks[p.x][p.y][p.z];
}

void Chunk::set(const ivec3& p, BlockState* bs)
{
    dirty = true;

    blocks[p.x][p.y][p.z] = *bs;

    if (p.x == 0 && west != NULL)
        west->dirty = true;
    else if (p.x == (CHUNK_WIDTH - 1) && east != NULL)
        east->dirty = true;

    if (p.z == 0 && south != NULL)
        south->dirty = true;
    else if (p.z == (CHUNK_WIDTH - 1) && north != NULL)
        north->dirty = true;
}

int Chunk::get_light(const ivec3& p)
{
    return (int)light[p.x][p.y][p.z];
}

void Chunk::set_light(const ivec3& p, int value)
{
    light[p.x][p.y][p.z] = (unsigned char)value;
}

int Chunk::get_sky_light(const ivec3& p)
{
    return (int)(light[p.x][p.y][p.z] & 0x0f);
}

void Chunk::set_sky_light(const ivec3& p, int value)
{
    light[p.x][p.y][p.z] = (light[p.x][p.y][p.z] & 0xf0) | ((unsigned char)value & 0x0f);
}

int Chunk::get_block_light(const ivec3& p)
{
    return (int)(light[p.x][p.y][p.z] >> 4);
}

void Chunk::set_block_light(const ivec3& p, int value)
{
    light[p.x][p.y][p.z] = (light[p.x][p.y][p.z] & 0x0f) | ((unsigned char)value << 4);
}

void Chunk::load()
{
    SimplexNoise noise;
    vec3 world_pos = chunk_pos_to_world(pos);

    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            vec3 p = (vec3(x, 0, z) + world_pos) * 0.002f;

            int n = noise.fractal(6, p.x, p.z) * 40 + 120;

            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                if (y < n)
                {
                    vec3 p2 = (vec3(x, y, z) + world_pos) * 0.01f;

                    float f = noise.fractal(3, p2.x, p2.y, p2.z);

                    if (f < 0.3f)
                    {
                        if (y == n - 1)
                            blocks[x][y][z].id = BlockID::GRASS_BLOCK;
                        else
                            blocks[x][y][z].id = BlockID::STONE;

                        /*if (y < n - 2)
                            blocks[x][y][z].id = BlockID::STONE;
                        else if (y < n)
                            blocks[x][y][z].id = BlockID::DIRT;
                        else if (y < n + 1)
                            blocks[x][y][z].id = BlockID::GRASS_BLOCK;
                        else
                        {
                            blocks[x][y][z].id = BlockID::AIR;

                            if (y - 1 >= 0 && blocks[x][y - 1][z].id == BlockID::GRASS_BLOCK)
                            {
                                if (random_int(1, 13) == 1)
                                    blocks[x][y][z].id = BlockID::GRASS;
                            }
                        }*/
                    }
                    else
                        blocks[x][y][z].id = BlockID::AIR;
                }
                else
                    blocks[x][y][z].id = BlockID::AIR;
            }
        }

    dirty = true;
    can_draw = true;
}

void Chunk::draw()
{
    if (!can_draw)
        return;

    mesh.draw();
}

void Chunk::destroy()
{
    mesh.destroy();

    if (north != NULL)
    {
        north->south = NULL;
        north->dirty = true;
    }

    if (south != NULL)
    {
        south->north = NULL;
        south->dirty = true;
    }

    if (east != NULL)
    {
        east->west = NULL;
        east->dirty = true;
    }

    if (west != NULL)
    {
        west->east = NULL;
        west->dirty = true;
    }

    delete this;
}

vec3 chunk_pos_to_world(const ivec2& pos)
{
    return vec3(pos.x, 0, pos.y) * (float)CHUNK_WIDTH;
}

dvec3 chunk_pos_to_world_hp(const ivec2& pos)
{
    return dvec3(pos.x, 0, pos.y) * (double)CHUNK_WIDTH;
}

ivec2 world_pos_to_chunk(const vec3& pos)
{
    return ivec2(floorf(pos.x / CHUNK_WIDTH), floorf(pos.z / CHUNK_WIDTH));
}

bool in_chunk(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT || z >= CHUNK_WIDTH)
        return false;

    return true;
}

bool in_chunk(const ivec3& pos)
{
    return in_chunk(pos.x, pos.y, pos.z);
}

Chunk* get_neighbor_chunk(Chunk* chunk, ivec3& pos)
{
    if (pos.x < 0)
    {
        pos.x += CHUNK_WIDTH;
        chunk = chunk->west;
    }
    else if (pos.x >= CHUNK_WIDTH)
    {
        pos.x -= CHUNK_WIDTH;
        chunk = chunk->east;
    }

    if (chunk->can_draw == false)
        return NULL;

    if (!chunk) return NULL;

    if (pos.z < 0)
    {
        pos.z += CHUNK_WIDTH;
        chunk = chunk->south;
    }
    else if (pos.z >= CHUNK_WIDTH)
    {
        pos.z -= CHUNK_WIDTH;
        chunk = chunk->north;
    }

    if (chunk->can_draw == false)
        return NULL;

    return chunk;
}