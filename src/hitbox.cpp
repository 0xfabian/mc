#include <hitbox.h>

Hitbox::Hitbox(const dvec3& _pos, const dvec3& _size)
{
    pos = _pos;
    size = _size;
}

void Hitbox::draw()
{
    vec3 vertices[24];
    vec3 v3pos = (vec3)pos;

    vertices[0] = v3pos;
    vertices[1] = v3pos + vec3(size.x, 0, 0);
    vertices[2] = v3pos + vec3(size.x, 0, 0);
    vertices[3] = v3pos + vec3(size.x, size.y, 0);
    vertices[4] = v3pos + vec3(size.x, size.y, 0);
    vertices[5] = v3pos + vec3(0, size.y, 0);
    vertices[6] = v3pos + vec3(0, size.y, 0);
    vertices[7] = v3pos;

    vertices[8] = v3pos + vec3(0, 0, size.z);
    vertices[9] = v3pos + vec3(size.x, 0, size.z);
    vertices[10] = v3pos + vec3(size.x, 0, size.z);
    vertices[11] = v3pos + vec3(size.x, size.y, size.z);
    vertices[12] = v3pos + vec3(size.x, size.y, size.z);
    vertices[13] = v3pos + vec3(0, size.y, size.z);
    vertices[14] = v3pos + vec3(0, size.y, size.z);
    vertices[15] = v3pos + vec3(0, 0, size.z);

    vertices[16] = v3pos;
    vertices[17] = v3pos + vec3(0, 0, size.z);
    vertices[18] = v3pos + vec3(size.x, 0, 0);
    vertices[19] = v3pos + vec3(size.x, 0, size.z);
    vertices[20] = v3pos + vec3(size.x, size.y, 0);
    vertices[21] = v3pos + vec3(size.x, size.y, size.z);
    vertices[22] = v3pos + vec3(0, size.y, 0);
    vertices[23] = v3pos + vec3(0, size.y, size.z);

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec3), vertices, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), NULL);
    glEnableVertexAttribArray(0);

    glLineWidth(3);
    glDrawArrays(GL_LINES, 0, 24);
    glLineWidth(1);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Hitbox get_bounds(const Hitbox& a, const Hitbox& b)
{
    double min_x = min(a.pos.x, b.pos.x);
    double min_y = min(a.pos.y, b.pos.y);
    double min_z = min(a.pos.z, b.pos.z);

    double max_x = max(a.pos.x + a.size.x, b.pos.x + b.size.x);
    double max_y = max(a.pos.y + a.size.y, b.pos.y + b.size.y);
    double max_z = max(a.pos.z + a.size.z, b.pos.z + b.size.z);

    dvec3 min_v = dvec3(min_x, min_y, min_z);
    dvec3 max_v = vec3(max_x, max_y, max_z);

    return Hitbox(min_v, max_v - min_v);
}

bool intersect(const Hitbox& a, const Hitbox& b)
{
    return (a.pos.x <= b.pos.x + b.size.x && a.pos.x + a.size.x >= b.pos.x) &&
        (a.pos.y <= b.pos.y + b.size.y && a.pos.y + a.size.y >= b.pos.y) &&
        (a.pos.z <= b.pos.z + b.size.z && a.pos.z + a.size.z >= b.pos.z);
}