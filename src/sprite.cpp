#include <sprite.h>

struct SpriteVertex
{
    vec2 p;
    vec2 uv;
};

Sprite::Sprite(const vec2& _pos, const vec2& _size, const vec2& _uv, Texture* _tex)
{
    pos = _pos;
    size = _size;
    uv = _uv;
    tex = _tex;

    build();
}

void Sprite::build()
{
    std::vector<SpriteVertex> vertices;
    std::vector<int> indices;

    SpriteVertex v;

    float scale = 3;

    v.p = vec2(0, 0);
    v.uv = vec2(uv.x / (float)tex->width, uv.y / (float)tex->height);
    vertices.push_back(v);

    v.p = vec2(size.x, 0) * scale;
    v.uv = vec2((uv.x + size.x) / (float)tex->width, uv.y / (float)tex->height);
    vertices.push_back(v);

    v.p = vec2(size.x, size.y) * scale;
    v.uv = vec2((uv.x + size.x) / (float)tex->width, (uv.y - size.y) / (float)tex->height);
    vertices.push_back(v);

    v.p = vec2(0, size.y) * scale;
    v.uv = vec2(uv.x / (float)tex->width, (uv.y - size.y) / (float)tex->height);
    vertices.push_back(v);

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SpriteVertex), vertices.data(), GL_STREAM_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, p));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)offsetof(SpriteVertex, uv));
    glEnableVertexAttribArray(1);

    indices_count = indices.size();
}

void Sprite::draw()
{
    gui_shader.bind();
    gui_shader.upload_texture("tex", *tex);
    gui_shader.upload_vec3("pos", vec3(pos.x, pos.y, 0));

    glBindVertexArray(vao);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
}

void Sprite::destroy()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}