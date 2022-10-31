#include <lighting.h>

struct light_source
{
    ivec3 pos;
    Chunk* chunk;
};

struct light_remove
{
    ivec3 pos;
    Chunk* chunk;
    char val;
};

void cast_sky_light(Chunk* chunk, int x, int z, int height)
{
    int light_ray = 15;

    for (int y = height; y >= 0; y--)
    {
        int id = chunk->blocks[x][y][z].id;

        Block* b = get_block_by_id(id);

        if (b->opacity > 0 || id == BlockID::LEAVES)
            light_ray = 0;

        chunk->light[x][y][z] = light_ray;
    }
}

void propagate_sky_light(std::queue<light_source>& q)
{
    while (!q.empty())
    {
        light_source src = q.front();
        q.pop();

        int light = src.chunk->get_sky_light(src.pos) - 1;

        for (int f = 0; f < 6; f++)
        {
            ivec3 pos = src.pos + face_normal[f];

            if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
                continue;

            Chunk* chunk = get_neighbor_chunk(src.chunk, pos);

            if (chunk == NULL)
                continue;

            chunk->dirty = true;

            Block* b = get_block_by_id(chunk->get(pos)->id);

            if (b->opacity > 0)
                continue;

            if (chunk->get_sky_light(pos) < light)
            {
                chunk->set_sky_light(pos, light);

                if (light > 1)
                    q.push((light_source) { pos, chunk });
            }
        }
    }
}

void propagate_block_light(std::queue<light_source>& q)
{
    while (!q.empty())
    {
        light_source src = q.front();
        q.pop();

        int light = src.chunk->get_block_light(src.pos) - 1;

        for (int f = 0; f < 6; f++)
        {
            ivec3 pos = src.pos + face_normal[f];

            if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
                continue;

            Chunk* chunk = get_neighbor_chunk(src.chunk, pos);

            if (chunk == NULL)
                continue;

            chunk->dirty = true;

            Block* b = get_block_by_id(chunk->get(pos)->id);

            if (b->opacity > 0)
                continue;

            if (chunk->get_block_light(pos) < light)
            {
                chunk->set_block_light(pos, light);

                if (light > 1)
                    q.push((light_source) { pos, chunk });
            }
        }
    }
}

void propagate_sky_darkness(std::queue<light_remove>& q)
{
    std::queue<light_source> q2;

    while (!q.empty())
    {
        light_remove rm = q.front();
        q.pop();

        for (int f = 0; f < 6; f++)
        {
            ivec3 n_pos = rm.pos + face_normal[f];

            if (n_pos.y < 0 || n_pos.y >= CHUNK_HEIGHT)
                continue;

            Chunk* chunk = get_neighbor_chunk(rm.chunk, n_pos);

            if (chunk == NULL)
                continue;

            chunk->dirty = true;

            if (rm.val > 0 && rm.val > chunk->get_sky_light(n_pos))
            {
                q.push((light_remove) { n_pos, chunk, (char)chunk->get_sky_light(n_pos) });
                chunk->set_sky_light(n_pos, 0);
            }
            else if (rm.val <= chunk->get_sky_light(n_pos))
            {
                q2.push((light_source) { n_pos, chunk });
            }
        }
    }

    propagate_sky_light(q2);
}

void propagate_block_darkness(Chunk* chunk, const ivec3& pos)
{
    std::queue<light_remove> q;
    std::queue<light_source> q2;

    q.push((light_remove) { pos, chunk, (char)chunk->get_block_light(pos) });

    chunk->set_block_light(pos, 0);

    while (!q.empty())
    {
        light_remove rm = q.front();
        q.pop();

        for (int f = 0; f < 6; f++)
        {
            ivec3 n_pos = rm.pos + face_normal[f];

            if (n_pos.y < 0 || n_pos.y >= CHUNK_HEIGHT)
                continue;

            Chunk* chunk = get_neighbor_chunk(rm.chunk, n_pos);

            if (chunk == NULL)
                continue;

            chunk->dirty = true;

            if (rm.val > 0 && rm.val > chunk->get_block_light(n_pos))
            {
                q.push((light_remove) { n_pos, chunk, (char)chunk->get_block_light(n_pos) });
                chunk->set_block_light(n_pos, 0);
            }
            else if (rm.val <= chunk->get_block_light(n_pos))
            {
                q2.push((light_source) { n_pos, chunk });
            }
        }
    }

    propagate_block_light(q2);
}

void propagate_light(Chunk* chunk)
{
    std::queue<light_source> light_sources;

    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int y = (CHUNK_HEIGHT - 1); y >= 0; y--)
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                if (chunk->light[x][y][z] == 0)
                {
                    for (int f = 0; f < 6; f++)
                    {
                        ivec3 pos = ivec3(x, y, z) + face_normal[f];

                        if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
                            continue;

                        Chunk* _chunk = get_neighbor_chunk(chunk, pos);

                        if (_chunk == NULL)
                            continue;

                        if (_chunk->get_light(pos) > 1)
                            light_sources.push((light_source) { pos, _chunk });
                    }
                }
            }

    propagate_sky_light(light_sources);
}

void calculate_lighting(Chunk* chunk)
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int z = 0; z < CHUNK_WIDTH; z++)
            cast_sky_light(chunk, x, z, CHUNK_HEIGHT - 1);

    propagate_light(chunk);
}

void update_lighting(Chunk* chunk, const ivec3& pos, bool placed, BlockState* bs)
{
    bool is_leaf = (bs->id == BlockID::LEAVES);

    Block* block = get_block_by_id(bs->id);

    if (placed)
    {
        if (block->opacity > 0 || is_leaf)
        {
            std::queue<light_remove> q;
            q.push((light_remove) { pos, chunk, (char)chunk->get_sky_light(pos) });
            chunk->set_sky_light(pos, 0);

            for (int y = pos.y - 1; y >= 0; y--)
            {
                ivec3 p = ivec3(pos.x, y, pos.z);

                Block* b = get_block_by_id(chunk->get(p)->id);

                if (b->opacity > 0 || chunk->get(p)->id == BlockID::LEAVES)
                    break;

                q.push((light_remove) { p, chunk, (char)chunk->get_sky_light(p) });
                chunk->set_sky_light(p, 0);
            }

            propagate_sky_darkness(q);
            propagate_block_darkness(chunk, pos);
        }

        if (block->light > 0)
        {
            chunk->set_block_light(pos, block->light);

            std::queue<light_source> q;
            q.push((light_source) { pos, chunk });

            propagate_block_light(q);
        }
    }
    else
    {
        if (block->light > 0)
            propagate_block_darkness(chunk, pos);

        if (block->opacity == 0 && !is_leaf)
            return;

        int block_light = 0;
        int sky_light = 0;

        for (int f = 0; f < 6; f++)
        {
            ivec3 n_pos = pos + face_normal[f];

            if (n_pos.y < 0 || n_pos.y >= CHUNK_HEIGHT)
                continue;

            Chunk* _chunk = get_neighbor_chunk(chunk, n_pos);

            if (_chunk == NULL)
                continue;

            if (_chunk->get_sky_light(n_pos) > sky_light)
                sky_light = _chunk->get_sky_light(n_pos);

            if (_chunk->get_block_light(n_pos) > block_light)
                block_light = _chunk->get_block_light(n_pos);
        }

        if (block_light > 0)
            block_light--;

        if (sky_light > 0)
            sky_light--;

        int y;

        for (y = pos.y + 1; y < CHUNK_HEIGHT; y++)
        {
            int id = chunk->blocks[pos.x][y][pos.z].id;

            Block* b = get_block_by_id(id);

            if (b->opacity > 0 || id == BlockID::LEAVES)
                break;
        }

        std::queue<light_source> sky_q;
        std::queue<light_source> block_q;

        if (y == CHUNK_HEIGHT)
        {
            sky_light = 15;

            for (y = pos.y - 1; y >= 0; y--)
            {
                int id = chunk->blocks[pos.x][y][pos.z].id;

                Block* b = get_block_by_id(id);

                if (b->opacity > 0 || id == BlockID::LEAVES)
                    break;

                chunk->set_sky_light(ivec3(pos.x, y, pos.z), sky_light);
                sky_q.push((light_source) { ivec3(pos.x, y, pos.z), chunk });
            }
        }

        chunk->set_sky_light(pos, sky_light);
        sky_q.push((light_source) { pos, chunk });

        chunk->set_block_light(pos, block_light);
        block_q.push((light_source) { pos, chunk });

        propagate_sky_light(sky_q);
        propagate_block_light(block_q);
    }
}