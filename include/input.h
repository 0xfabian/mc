#pragma once

#define KEY_MAX         512
#define BUTTON_MAX      3

enum Input
{
    NONE,
    DOWN,
    PRESSED,
    UP
};

extern int keys[];
extern int buttons[];

void update_inputs();

bool is_key_down(int keycode);
bool is_key_pressed(int keycode);
bool is_key_up(int keycode);

bool is_button_down(int button);
bool is_button_pressed(int button);
bool is_button_up(int buttom);