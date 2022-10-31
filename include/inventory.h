#pragma once

#include <slot.h>

struct Inventory
{
    Slot slots[6][9];
    Slot* over;

    Sprite s_inventory;
    Sprite s_highlight;

    Inventory();

    void set_item(int i, int j, int id);
    int get_item(int i, int j);

    void update();
    void draw();
};