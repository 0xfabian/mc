#pragma once

#include <game.h>

void toggle_mouse_control()
{
    mouse_control = !mouse_control;

    SDL_ShowCursor(!mouse_control);
    SDL_SetRelativeMouseMode((SDL_bool)mouse_control);
    SDL_WarpMouseInWindow(win, win_size.x / 2, win_size.y / 2);
}

void toggle_fullscreen()
{
    is_fullscreen = !is_fullscreen;

    if (is_fullscreen)
    {
        win_size = uvec2(1920, 1080);
        SDL_SetWindowSize(win, win_size.x, win_size.y);
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        win_size = uvec2(800, 600);
        SDL_SetWindowFullscreen(win, 0);
        SDL_SetWindowSize(win, win_size.x, win_size.y);
    }

    glViewport(0, 0, win_size.x, win_size.y);
    player->cam.aspect_ratio = win_size.x / (float)win_size.y;

    font_shader.bind();
    font_shader.upload_int("win_width", win_size.x);
    font_shader.upload_int("win_height", win_size.y);

    gui_shader.bind();
    gui_shader.upload_int("win_width", win_size.x);
    gui_shader.upload_int("win_height", win_size.y);
}

void handle_misc_keys()
{
    if (is_key_down(SDLK_ESCAPE))
    {
        if (show_inventory)
        {
            show_inventory = false;
            toggle_mouse_control();
        }
    }

    if (is_key_down(SDLK_F11))
        toggle_fullscreen();

    if (is_key_down(SDLK_F3))
        show_debug = !show_debug;

    if (is_key_down(SDLK_F5))
        show_wireframe = !show_wireframe;

    if (is_key_down(SDLK_F6))
        show_ao = !show_ao;

    if (is_key_down(SDLK_RIGHTBRACKET))
    {
        if (global_light < 15)
            global_light++;
    }

    if (is_key_down(SDLK_LEFTBRACKET))
    {
        if (global_light > 0)
            global_light--;
    }

    if (is_key_down(SDLK_i))
    {
        show_inventory = !show_inventory;

        if (mouse_control == show_inventory)
            toggle_mouse_control();
    }

    if (is_key_down(SDLK_TAB))
        survival_mode = !survival_mode;
}

