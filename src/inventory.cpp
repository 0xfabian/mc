#include <inventory.h>

Inventory::Inventory()
{
    s_inventory = Sprite(vec2(-176 / 2 * 3, -122 / 2 * 3), vec2(176, 122), vec2(0, 228), &gui);
    s_highlight = Sprite(vec2(0, 0), vec2(16, 16), vec2(0, 256), &gui);

    vec2 off = s_inventory.pos + vec2(0, 122 * 3 - 18 * 3) + vec2(8 * 3, -6 * 3);

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 9; j++)
            slots[i][j] = Slot(off + vec2(j, -i) * 18.0f * 3.0f);

    over = NULL;
}

void Inventory::set_item(int i, int j, int id)
{
    slots[i][j].set_item(id);
}

int Inventory::get_item(int i, int j)
{
    return slots[i][j].get_item();
}

void Inventory::update()
{
    over = NULL;

    ivec2 int_mouse;

    SDL_GetMouseState(&int_mouse.x, &int_mouse.y);

    vec2 mouse = (vec2)int_mouse - 0.5f * (vec2)win_size;
    mouse.y *= -1;

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 9; j++)
            if (slots[i][j].is_point_over(mouse))
            {
                over = &slots[i][j];
                return;
            }
}

void Inventory::draw()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    s_inventory.draw();

    glClear(GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 9; j++)
            slots[i][j].draw();

    if (over != NULL)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        s_highlight.pos = over->pos;
        s_highlight.draw();
    }
}