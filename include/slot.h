#pragma once

#include <global.h>
#include <sprite.h>
#include <blocks.h>

struct Slot
{
    vec2 pos;

    int item_id;
    Sprite* item_icon;

    Slot() {}
    Slot(const vec2& _pos);

    bool is_empty();
    int get_item();
    void set_item(int id);
    bool is_point_over(vec2 p);

    void draw();
};