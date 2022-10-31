#include <hotbar.h>

Hotbar::Hotbar()
{
    s_hotbar = Sprite(vec2(-(182 * 3) / 2, -1080 / 2), vec2(182, 22), vec2(0, 22), &gui);

    s_selector = Sprite(vec2(0, 0), vec2(24, 24), vec2(0, 22 + 24), &gui);
    s_selector.pos = s_hotbar.pos + vec2(-3, -3);

    index = 0;

    for (int i = 0; i < 9; i++)
        slots[i] = Slot(s_hotbar.pos + vec2(3 * 3, 3 * 3) + vec2(i * 3 * 20, 0));
}

void Hotbar::set_index(int _index)
{
    index = _index;

    s_selector.pos = s_hotbar.pos + vec2(-3, -3) + vec2(index * 3 * 20, 0);
}

int Hotbar::get_index()
{
    return index;
}

void Hotbar::set_item(int id)
{
    slots[index].set_item(id);
}

void Hotbar::set_item(int _index, int id)
{
    slots[_index].set_item(id);
}

int Hotbar::get_item()
{
    return slots[index].get_item();
}

int Hotbar::get_item(int _index)
{
    return slots[_index].get_item();
}

int Hotbar::find_item(int id)
{
    for (int i = 0; i < 9; i++)
        if (slots[i].get_item() == id)
            return i;

    return -1;
}

int Hotbar::find_empty()
{
    for (int i = 0; i < 9; i++)
        if (slots[i].is_empty())
            return i;

    return -1;
}

bool Hotbar::is_empty()
{
    return slots[index].is_empty();
}

bool Hotbar::is_empty(int _index)
{
    return slots[_index].is_empty();
}

void Hotbar::draw()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    s_hotbar.draw();

    glClear(GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < 9; i++)
        slots[i].draw();

    glClear(GL_DEPTH_BUFFER_BIT);
    s_selector.draw();
}