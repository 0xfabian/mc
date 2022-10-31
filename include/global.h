#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <glad/glad.h>
#include <glm.hpp>
#include <shader.h>
#include <input.h>

using namespace glm;

extern SDL_Window* win;
extern uvec2 win_size;
extern char win_name[];

extern bool closed;
extern bool mouse_control;
extern bool is_fullscreen;
extern bool show_wireframe;
extern bool show_ao;
extern bool show_debug;
extern bool show_inventory;
extern bool survival_mode;

extern double delta_time;

extern Shader line_shader;
extern Shader block_shader;
extern Shader font_shader;
extern Shader gui_shader;

extern Texture texture_atlas;
extern Texture gui;

struct Player;
extern Player* player;

struct World;
extern World* world;

extern int global_light;