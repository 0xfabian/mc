#include <blocks.h>

#define RIGHT_FACE  0
#define LEFT_FACE   1
#define TOP_FACE    2
#define BOTTOM_FACE 3
#define BACK_FACE   4
#define FRONT_FACE  5

ivec3 vertex_offset[6][4] =
{
    { ivec3(1, 0, 0), ivec3(1, 0, 1), ivec3(1, 1, 1), ivec3(1, 1, 0) },
    { ivec3(0, 0, 1), ivec3(0, 0, 0), ivec3(0, 1, 0), ivec3(0, 1, 1) },
    { ivec3(0, 1, 0), ivec3(1, 1, 0), ivec3(1, 1, 1), ivec3(0, 1, 1) },
    { ivec3(0, 0, 1), ivec3(1, 0, 1), ivec3(1, 0, 0), ivec3(0, 0, 0) },
    { ivec3(1, 0, 1), ivec3(0, 0, 1), ivec3(0, 1, 1), ivec3(1, 1, 1) },
    { ivec3(0, 0, 0), ivec3(1, 0, 0), ivec3(1, 1, 0), ivec3(0, 1, 0) }
};

const vec3 cross_vertex_offset[] =
{
    vec3(0, 0, 0),
    vec3(1, 0, 1),
    vec3(1, 0.875f, 1),
    vec3(0, 0.875f, 0),

    vec3(1, 0, 1),
    vec3(0, 0, 0),
    vec3(0, 0.875f, 0),
    vec3(1, 0.875f, 1),

    vec3(0, 0, 1),
    vec3(1, 0, 0),
    vec3(1, 0.875f, 0),
    vec3(0, 0.875f, 1),

    vec3(1, 0, 0),
    vec3(0, 0, 1),
    vec3(0, 0.875f, 1),
    vec3(1, 0.875f, 0)
};

ivec3 ao_offset[6][4] =
{
    { ivec3(0, 0, -1), ivec3(0, -1, 0), ivec3(0, 0, 1), ivec3(0, 1, 0) },
    { ivec3(0, 0, 1), ivec3(0, -1, 0), ivec3(0, 0, -1), ivec3(0, 1, 0) },
    { ivec3(-1, 0, 0), ivec3(0, 0, -1), ivec3(1, 0, 0), ivec3(0, 0, 1) },
    { ivec3(-1, 0, 0), ivec3(0, 0, 1), ivec3(1, 0, 0), ivec3(0, 0, -1) },
    { ivec3(1, 0, 0), ivec3(0, -1, 0), ivec3(-1, 0, 0), ivec3(0, 1, 0) },
    { ivec3(-1, 0, 0), ivec3(0, -1, 0), ivec3(1, 0, 0), ivec3(0, 1, 0) }
};

ivec3 face_normal[] =
{
    ivec3(1, 0, 0),
    ivec3(-1, 0, 0),
    ivec3(0, 1, 0),
    ivec3(0, -1, 0),
    ivec3(0, 0, 1),
    ivec3(0, 0, -1)
};

vec2 uv_offsets[4] =
{
    vec2(0.0f, 1 / 32.0f),
    vec2(1 / 32.0f, 1 / 32.0f),
    vec2(1 / 32.0f, 0.0f),
    vec2(0.0f, 0.0f)
};

Block* blocks[20];

/*bool add_block_light(Chunk* chunk, ivec3& pos, int& sky_sum, int& block_sum, int& n)
{
    if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
        return false;

    chunk = get_neighbor_chunk(chunk, pos);

    if (chunk == NULL)
        return false;

    if (chunk->get(pos).id != BlockID::AIR && chunk->get(pos).id != BlockID::GLASS)
        return false;

    sky_sum += chunk->get_sky_light(pos);
    block_sum += chunk->get_block_light(pos);
    n++;

    return true;
}

int get_vertex_light(Chunk* chunk, const ivec3& pos, int face, int vertex)
{
    ivec3 normal = pos + face_normal[face];
    ivec3 side1 = pos + face_normal[face] + ao_offset[face][vertex];
    ivec3 side2 = pos + face_normal[face] + ao_offset[face][(vertex + 1) & 3];
    ivec3 corner = pos + face_normal[face] + ao_offset[face][vertex] + ao_offset[face][(vertex + 1) & 3];

    int sky_sum = 0;
    int block_sum = 0;
    int n = 0;

    add_block_light(chunk, normal, sky_sum, block_sum, n);

    bool s1 = add_block_light(chunk, side1, sky_sum, block_sum, n);
    bool s2 = add_block_light(chunk, side2, sky_sum, block_sum, n);

    if (s1 || s2)
        add_block_light(chunk, corner, sky_sum, block_sum, n);

    int sky = sky_sum / (float)n;
    int block = block_sum / (float)n;

    return (sky & 0x0f) | ((block & 0x0f) << 4);
}*/

bool is_block(Chunk* chunk, ivec3& pos)
{
    if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
        return false;

    chunk = get_neighbor_chunk(chunk, pos);

    if (chunk == NULL)
        return false;

    int id = chunk->get(pos)->id;

    Block* b = get_block_by_id(id);

    if (id == BlockID::LEAVES)
        return true;

    return b->opacity > 0;
}

int get_ao(Chunk* chunk, const ivec3& pos, int face, int vertex)
{
    ivec3 pos1 = pos + face_normal[face] + ao_offset[face][vertex];
    ivec3 pos2 = pos + face_normal[face] + ao_offset[face][(vertex + 1) & 3];
    ivec3 pos3 = pos + face_normal[face] + ao_offset[face][vertex] + ao_offset[face][(vertex + 1) & 3];

    int side1 = is_block(chunk, pos1);
    int side2 = is_block(chunk, pos2);
    int corner = is_block(chunk, pos3);

    if (side1 && side2)
        return 3;

    return side1 + side2 + corner;
}

bool face_is_visible(Chunk* chunk, const ivec3& pos, int face, int& light)
{
    ivec3 target_pos = pos + face_normal[face];

    if (target_pos.y >= CHUNK_HEIGHT)
    {
        light = 15;
        return true;
    }

    if (target_pos.y < 0)
        return false;

    Chunk* _chunk = get_neighbor_chunk(chunk, target_pos);

    if (_chunk == NULL)
        return false;

    light = _chunk->get_light(target_pos);

    BlockState* bs = _chunk->get(target_pos);
    Block* b = get_block_by_id(bs->id);

    if (chunk->get(pos)->id == GLASS && bs->id == GLASS)
        return false;

    return b->opacity == 0;
}

void build_cube(Block* block, Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
{
    for (int f = 0; f < 6; f++)
    {
        int light;

        if (face_is_visible(chunk, pos, f, light))
        {
            vec2 tex_coord = block->get_texture(chunk, pos, f);
            vec3 vert_pos = (vec3)pos + chunk_pos_to_world(chunk->pos);

            for (int v = 0; v < 4; v++)
            {
                int ao = get_ao(chunk, pos, f, v);

                Vertex vert = Vertex(vert_pos + (vec3)vertex_offset[f][v], f, light, ao, tex_coord, v);

                vertices.push_back(vert);
            }

            indices.push_back(index + 0);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            indices.push_back(index + 0);
            indices.push_back(index + 2);
            indices.push_back(index + 3);

            index += 4;
        }
    }
}

void build_oriented_cube(Block* block, Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
{
    int orientation = chunk->get(pos)->data1 & 3;

    for (int f = 0; f < 6; f++)
    {
        int light;

        int r = 0;

        if (orientation == 0) // x
        {
            if (f != RIGHT_FACE && f != LEFT_FACE)
                r = 1;
        }
        else if (orientation == 2) // z
        {
            if (f == RIGHT_FACE || f == LEFT_FACE)
                r = 1;
        }

        if (face_is_visible(chunk, pos, f, light))
        {
            vec2 tex_coord = block->get_texture(chunk, pos, f);
            vec3 vert_pos = (vec3)pos + chunk_pos_to_world(chunk->pos);

            for (int v = 0; v < 4; v++)
            {
                int ao = get_ao(chunk, pos, f, (v + r) & 3);

                Vertex vert = Vertex(vert_pos + (vec3)vertex_offset[f][(v + r) & 3], f, light, ao, tex_coord, v);

                vertices.push_back(vert);
            }

            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            indices.push_back(index + 0);
            indices.push_back(index + 2);
            indices.push_back(index + 3);

            index += 4;
        }
    }
}

void build_cross(Block* block, Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
{
    vec2 tex_coord = block->get_texture(chunk, pos, 0);
    int light = chunk->get_light(pos);
    vec3 vert_pos = (vec3)pos + chunk_pos_to_world(chunk->pos);

    for (int i = 0; i < 16; i++)
    {
        Vertex vert = Vertex(vert_pos + (vec3)cross_vertex_offset[i], TOP_FACE, light, 0, tex_coord, i % 4);

        vertices.push_back(vert);
    }

    for (int i = 0; i < 4; i++)
    {
        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);

        indices.push_back(index + 0);
        indices.push_back(index + 2);
        indices.push_back(index + 3);

        index += 4;
    }
}

void build_torch(Block* block, Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
{
    int orientation = chunk->get(pos)->data1 & 7;

    for (int f = 0; f < 6; f++)
    {
        if (f == BOTTOM_FACE)
            continue;

        int light = chunk->get_light(pos);

        vec2 tex_coord = block->get_texture(chunk, pos, f);
        vec3 vert_pos = (vec3)pos + chunk_pos_to_world(chunk->pos);

        for (int v = 0; v < 4; v++)
        {
            vec3 offset = (vec3)vertex_offset[f][v] - (vec3)face_normal[f] * (7 / 16.0f);

            if (f == TOP_FACE)
                offset += vec3(0, 1 / 16.0f, 0);

            if (orientation != 0)
            {
                offset -= vec3(0.5f);

                vec3 normal;

                if (orientation == 1)
                {
                    offset = rotateX(offset, radians(24.0f));
                    normal = vec3(0, 0, 1);
                }
                else if (orientation == 2)
                {
                    offset = rotateX(offset, radians(-24.0f));
                    normal = vec3(0, 0, -1);
                }
                else if (orientation == 3)
                {
                    offset = rotateZ(offset, radians(-24.0f));
                    normal = vec3(1, 0, 0);
                }
                else
                {
                    offset = rotateZ(offset, radians(24.0f));
                    normal = vec3(-1, 0, 0);
                }

                offset += vec3(0.5f);
                offset += vec3(0, 0.15f, 0);
                offset -= normal * 0.295f;
            }

            Vertex vert = Vertex(vert_pos + offset, TOP_FACE, light, 0, tex_coord, v);

            vertices.push_back(vert);
        }

        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);

        indices.push_back(index + 0);
        indices.push_back(index + 2);
        indices.push_back(index + 3);

        index += 4;
    }
}

struct Air : public Block
{
    Air()
    {
        opacity = 0;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(0, 0), vec2(0, 0), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return;
    }
};

struct Stone : public Block
{
    Stone()
    {
        opacity = 15;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(0, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(0, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Dirt : public Block
{
    Dirt()
    {
        opacity = 15;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(16, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(1, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Grass_Block : public Block
{
    Grass_Block()
    {
        opacity = 15;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(32, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        if (face == TOP_FACE)
            return vec2(3, 0);

        if (face == BOTTOM_FACE)
            return vec2(1, 0);

        return vec2(2, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Log : public Block
{
    Log()
    {
        opacity = 15;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(6 * 16, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        BlockState* bs = chunk->get(pos);

        int orientation = 2 * (bs->data1 & 3);

        if (face == orientation || face == (orientation + 1))
            return vec2(7, 0);
        else
            return vec2(6, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_oriented_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Leaves : public Block
{
    Leaves()
    {
        opacity = 0;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(8 * 16, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(8, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Glass : public Block
{
    Glass()
    {
        opacity = 0;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(10 * 16, 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(10, 0);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

struct Grass : public Block
{
    Grass()
    {
        opacity = 0;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(0, 2 * 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(0, 1);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        build_cross(this, chunk, pos, vertices, indices, index);
    }
};

struct Torch : public Block
{
    Torch()
    {
        opacity = 0;
        light = 14;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(16, 2 * 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        if (face == TOP_FACE)
            return vec2(2, 1);
        else
            return vec2(1, 1);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_torch(this, chunk, pos, vertices, indices, index);
    }
};

struct Planks : public Block
{
    Planks()
    {
        opacity = 15;
        light = 0;
        icon = Sprite(vec2(0, 0), vec2(16, 16), vec2(6 * 16, 2 * 16), &texture_atlas);
    }

    vec2 get_texture(Chunk* chunk, const ivec3& pos, int face)
    {
        return vec2(6, 1);
    }

    void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index)
    {
        return build_cube(this, chunk, pos, vertices, indices, index);
    }
};

void blocks_init()
{
    blocks[AIR] = new Air();
    blocks[STONE] = new Stone();
    blocks[DIRT] = new Dirt();
    blocks[GRASS_BLOCK] = new Grass_Block();
    blocks[LOG] = new Log();
    blocks[LEAVES] = new Leaves();
    blocks[GLASS] = new Glass();
    blocks[GRASS] = new Grass();
    blocks[TORCH] = new Torch();
    blocks[PLANKS] = new Planks();
}

Block* get_block_by_id(int id)
{
    return blocks[id];
}