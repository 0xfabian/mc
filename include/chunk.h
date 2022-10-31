#pragma once

#include <global.h>
#include <vector>
#include <misc.h>
#include <SimplexNoise.h>
#include <blocks.h>
#include <vertex.h>

#define CHUNK_WIDTH     16
#define CHUNK_HEIGHT    256

struct Chunk;

struct ChunkMesh
{
    Chunk* chunk;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int indices_count;

    ChunkMesh();
    ChunkMesh(Chunk* chunk);

    void build();
    void draw();
    void destroy();
};

struct BlockState
{
    char id;
    char data1;
    char data2;
};

struct Chunk
{
    ivec2 pos;

    BlockState blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];
    unsigned char light[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];

    Chunk* north;
    Chunk* south;
    Chunk* east;
    Chunk* west;

    bool dirty;
    bool can_draw;

    ChunkMesh mesh;

    Chunk(const ivec2& _pos);

    BlockState* get(const ivec3& p);
    void set(const ivec3& p, BlockState* bs);

    int get_light(const ivec3& p);
    int get_sky_light(const ivec3& p);
    int get_block_light(const ivec3& p);

    void set_light(const ivec3& p, int value);
    void set_sky_light(const ivec3& p, int value);
    void set_block_light(const ivec3& p, int value);

    void load();
    void draw();
    void destroy();
};

vec3 chunk_pos_to_world(const ivec2& pos);
dvec3 chunk_pos_to_world_hp(const ivec2& pos);
ivec2 world_pos_to_chunk(const vec3& pos);

bool face_is_visible(Chunk* chunk, const ivec3& pos, int face, int& light);
bool in_chunk(int x, int y, int z);
bool in_chunk(const ivec3& pos);
Chunk* get_neighbor_chunk(Chunk* chunk, ivec3& pos);