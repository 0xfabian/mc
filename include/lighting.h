#pragma once

#include <chunk.h>
#include <queue>

void calculate_lighting(Chunk* chunk);
void update_lighting(Chunk* chunk, const ivec3& pos, bool placed, BlockState* bs);