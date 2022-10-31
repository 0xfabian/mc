#include <world.h>

#define RENDER_DISTANCE 16

SDL_Thread* world_gen_thread;
SDL_mutex* mut;

int world_gen_thread_fn(void* _world)
{
    World* world = (World*)_world;

    while (!closed)
        world->update2();

    return 0;
}

World::World()
{
    world_gen_thread = SDL_CreateThread(world_gen_thread_fn, "World Gen", this);
    mut = SDL_CreateMutex();

    old_p = ivec2(10);
}

Chunk* World::get_chunk_from_pos(const vec3 pos)
{
    ivec2 target = world_pos_to_chunk(pos);

    if (chunk_map.find(target) != chunk_map.end())
        return chunk_map[target];

    return NULL;
}

void World::remove_block(Chunk* chunk, const ivec3& pos)
{
    BlockState bs = *chunk->get(pos);
    BlockState air = { BlockID::AIR, 0, 0 };

    chunk->set(pos, &air);
    update_lighting(chunk, pos, false, &bs);
}

bool World::place_block(Chunk* chunk, const ivec3& pos, BlockState* bs)
{
    if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
        return false;

    ivec3 _pos = pos;

    Chunk* _chunk = get_neighbor_chunk(chunk, _pos);

    if (_chunk == NULL)
        return false;

    if (_chunk->get(_pos)->id != BlockID::AIR)
        return false;

    _chunk->set(_pos, bs);
    update_lighting(_chunk, _pos, true, bs);

    return true;
}

void World::update()
{
    ivec2 p = world_pos_to_chunk(player->pos);

    if (p != old_p)
    {
        old_p = p;

        //uint64_t t = SDL_GetPerformanceCounter();

        for (auto it = chunk_vector.begin(); it != chunk_vector.end();)
        {
            Chunk* chunk = *it;

            if (chebyshev_distance(p, chunk->pos) > RENDER_DISTANCE)
            {
                chunk_map.erase(chunk->pos);

                //chunk->destroy();

                it = chunk_vector.erase(it);
            }
            else
                it++;
        }

        for (int x = old_p.x - RENDER_DISTANCE; x <= old_p.x + RENDER_DISTANCE; x++)
            for (int z = old_p.y - RENDER_DISTANCE; z <= old_p.y + RENDER_DISTANCE; z++)
            {
                ivec2 pos = ivec2(x, z);

                if (chunk_map.find(pos) == chunk_map.end())
                {
                    Chunk* chunk = new Chunk(pos);

                    ivec2 north = ivec2(pos.x, pos.y + 1);
                    ivec2 south = ivec2(pos.x, pos.y - 1);
                    ivec2 east = ivec2(pos.x + 1, pos.y);
                    ivec2 west = ivec2(pos.x - 1, pos.y);

                    if (chunk_map.find(north) != chunk_map.end())
                    {
                        chunk_map[north]->south = chunk;
                        chunk->north = chunk_map[north];
                    }

                    if (chunk_map.find(south) != chunk_map.end())
                    {
                        chunk_map[south]->north = chunk;
                        chunk->south = chunk_map[south];
                    }

                    if (chunk_map.find(east) != chunk_map.end())
                    {
                        chunk_map[east]->west = chunk;
                        chunk->east = chunk_map[east];
                    }

                    if (chunk_map.find(west) != chunk_map.end())
                    {
                        chunk_map[west]->east = chunk;
                        chunk->west = chunk_map[west];
                    }

                    chunk_map[pos] = chunk;
                    chunk_vector.push_back(chunk);

                    if (SDL_LockMutex(mut) == 0)
                    {
                        load_q.push(chunk);

                        SDL_UnlockMutex(mut);
                    }
                }
            }

        // double ms = (SDL_GetPerformanceCounter() - t) / (double)SDL_GetPerformanceFrequency() * 1000.0;
         //printf("%lf ms\n", ms);
    }

    /*if (!load_q.empty())
    {
        Chunk* chunk = load_q.front();
        load_q.pop();

        chunk->load();
        calculate_lighting(chunk);
    }*/

    if (dirty_chunks.empty())
    {
        for (auto& it : chunk_vector)
            if (it->dirty)
                dirty_chunks.push(it);
    }
    else
    {
        Chunk* chunk = dirty_chunks.front();
        dirty_chunks.pop();

        chunk->mesh.destroy();
        chunk->mesh.build();
        chunk->dirty = false;
    }
}

void World::update2()
{
    Chunk* chunk = NULL;

    if (SDL_LockMutex(mut) == 0)
    {
        if (!load_q.empty())
        {
            chunk = load_q.front();
            load_q.pop();
        }

        SDL_UnlockMutex(mut);
    }

    if (chunk != NULL)
    {
        chunk->load();
        calculate_lighting(chunk);
    }
}

void World::draw()
{
    mat4 cam_mat = player->cam.matrix(player->pos);

    block_shader.bind();
    block_shader.upload_mat4("cam_mat", cam_mat);
    block_shader.upload_vec3("eye_pos", player->cam.pos + (vec3)player->pos);
    block_shader.upload_int("global_light", global_light);
    block_shader.upload_int("show_ao", (int)show_ao);

    for (auto& it : chunk_vector)
        it->draw();

    if (show_debug)
    {
        Chunk* pchunk = get_chunk_from_pos(player->pos);

        Hitbox box = Hitbox(chunk_pos_to_world(pchunk->pos), vec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH));

        line_shader.bind();
        line_shader.upload_mat4("cam_mat", cam_mat);
        line_shader.upload_vec4("color", vec4(1));

        box.draw();
    }
}

void World::destroy()
{
    SDL_WaitThread(world_gen_thread, NULL);
    SDL_DestroyMutex(mut);

    //for (auto& it : chunk_map)
        //it.second->destroy();

    delete this;
}

/*
void plant_tree(Chunk* chunk, const ivec3& pos)
{
    int height = random_int(5, 6);

    for (int y = pos.y; y < pos.y + height; y++)
    {
        BlockState bs = { BlockID::LOG, 1, 0 };
        chunk->set(ivec3(pos.x, y, pos.z), bs);
    }

    for (int x = pos.x - 2; x <= pos.x + 2; x++)
        for (int z = pos.z - 2; z <= pos.z + 2; z++)
        {
            for (int i = 0; i < 2; i++)
            {
                if ((x == pos.x - 2 || x == pos.x + 2) && (z == pos.z - 2 || z == pos.z + 2))
                {
                    if (random_int(1, 3) != 1)
                        continue;
                }

                ivec3 p = ivec3(x, pos.y + height - 2 - i, z);

                Chunk* c = get_neighbor_chunk(chunk, p);

                if (c == NULL)
                    continue;

                if (c->get(p)->id != BlockID::AIR)
                    continue;

                BlockState bs = { BlockID::LEAVES, 0, 0 };

                c->set(p, bs);
            }
        }

    for (int x = pos.x - 1; x <= pos.x + 1; x++)
        for (int z = pos.z - 1; z <= pos.z + 1; z++)
        {
            for (int i = 0; i < 2; i++)
            {
                if ((x == pos.x - 1 || x == pos.x + 1) && (z == pos.z - 1 || z == pos.z + 1))
                {
                    if (i == 0)
                        continue;

                    if (random_int(1, 2) != 1)
                        continue;
                }

                ivec3 p = ivec3(x, pos.y + height - i, z);

                Chunk* c = get_neighbor_chunk(chunk, p);

                if (c == NULL)
                    continue;

                if (c->get(p)->id != BlockID::AIR)
                    continue;

                BlockState bs = { BlockID::LEAVES, 0, 0 };

                c->set(p, bs);
            }
        }
}
*/