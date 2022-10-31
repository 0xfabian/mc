#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glm.hpp>

char* read_from_file(const char* path);

int random_int(int a, int b);

int manhattan_distance(const glm::ivec2& a, const glm::ivec2& b);
int chebyshev_distance(const glm::ivec2& a, const glm::ivec2& b);