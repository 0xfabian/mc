#include <misc.h>

char* read_from_file(const char* path)
{
    FILE* f = fopen(path, "rb");

    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = (char*)malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = 0;

    fclose(f);

    return data;
}

int random_int(int a, int b)
{
    return (rand() % (b - a + 1)) + a;
}

int manhattan_distance(const glm::ivec2& a, const glm::ivec2& b)
{
    glm::ivec2 diff = a - b;

    return abs(diff.x) + abs(diff.y);
}

int chebyshev_distance(const glm::ivec2& a, const glm::ivec2& b)
{
    glm::ivec2 diff = a - b;

    int x = abs(diff.x);
    int y = abs(diff.y);

    if (x > y)
        return x;

    return y;
}