#include <input.h>

int keys[KEY_MAX];
int buttons[BUTTON_MAX];

void update_inputs()
{
    for (int i = 0; i < KEY_MAX; i++)
    {
        if (keys[i] == Input::DOWN)
            keys[i] = Input::PRESSED;
        else if (keys[i] == Input::UP)
            keys[i] = Input::NONE;
    }

    for (int i = 0; i < BUTTON_MAX; i++)
    {
        if (buttons[i] == Input::DOWN)
            buttons[i] = Input::PRESSED;
        else if (buttons[i] == Input::UP)
            buttons[i] = Input::NONE;
    }
}

bool is_key_down(int keycode)
{
    keycode &= ~0x40000000;

    return keys[keycode] == Input::DOWN;
}

bool is_key_pressed(int keycode)
{
    keycode &= ~0x40000000;

    return keys[keycode] == Input::PRESSED;
}

bool is_key_up(int keycode)
{
    keycode &= ~0x40000000;

    return keys[keycode] == Input::UP;
}

bool is_button_down(int button)
{
    return buttons[button] == Input::DOWN;
}

bool is_button_pressed(int button)
{
    return buttons[button] == Input::PRESSED;
}

bool is_button_up(int button)
{
    return buttons[button] == Input::UP;
}