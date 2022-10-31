#pragma once

#include <slot.h>

struct Hotbar
{
    int index;
    Slot slots[9];

    Sprite s_hotbar;
    Sprite s_selector;

    Hotbar();

    void set_index(int _index);
    int get_index();

    void set_item(int id);
    void set_item(int _index, int id);
    int get_item();
    int get_item(int _index);

    int find_item(int id);
    int find_empty();

    bool is_empty();
    bool is_empty(int _index);

    void draw();
};