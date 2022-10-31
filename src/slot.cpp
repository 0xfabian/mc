#include <slot.h>

Slot::Slot(const vec2& _pos)
{
    pos = _pos;

    item_id = -1;
    item_icon = NULL;
}

bool Slot::is_empty()
{
    return item_id == -1;
}

int Slot::get_item()
{
    return item_id;
}

void Slot::set_item(int id)
{
    item_id = id;

    if (id != -1)
        item_icon = &get_block_by_id(id)->icon;
    else
        item_icon = NULL;
}

bool Slot::is_point_over(vec2 p)
{
    return (pos.x - 3) <= p.x && p.x < (pos.x + 17 * 3) && (pos.y - 3) <= p.y && p.y < (pos.y + 17 * 3);
}

void Slot::draw()
{
    if (item_id == -1 || item_icon == NULL)
        return;

    item_icon->pos = pos;
    item_icon->draw();
}