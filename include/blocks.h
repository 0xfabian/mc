#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>
#include <vertex.h>
#include <chunk.h>
#include <sprite.h>

using namespace glm;

enum BlockID
{
    AIR,
    STONE,
    DIRT,
    GRASS_BLOCK,
    SAND,
    LOG,
    LEAVES,
    GLOWSTONE,
    WATER,
    GLASS,
    GRASS,
    TORCH,
    PLANKS
};

struct Chunk;
struct BlockState;

struct Block
{
    char opacity;
    char light;
    Sprite icon;

    virtual vec2 get_texture(Chunk* chunk, const ivec3& pos, int face) = 0;
    virtual void build(Chunk* chunk, const ivec3& pos, std::vector<Vertex>& vertices, std::vector<int>& indices, int& index) = 0;
};

extern Block* blocks[];
extern ivec3 face_normal[];

void blocks_init();

Block* get_block_by_id(int id);


