#include <game.h>
#include <vector>
#include <keys.h>

SDL_Window* win;
uvec2 win_size = uvec2(800, 600);
char win_name[] = "mc";

bool closed = false;
bool mouse_control = false;
bool is_fullscreen = false;
bool show_wireframe = false;
bool show_ao = true;
bool show_debug = true;
bool show_inventory = false;
bool survival_mode = false;

double delta_time;

Shader line_shader;
Shader block_shader;
Shader font_shader;
Shader gui_shader;

Texture texture_atlas;
Texture day_lightmap;
Texture night_lightmap;
Texture font;
Texture gui;

Player* player;
World* world;

Text debug_text;
Sprite crosshair;

int global_light = 15;

void Game::init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    line_shader = Shader("shaders/line_vert.glsl", "shaders/line_frag.glsl");
    block_shader = Shader("shaders/block_vert.glsl", "shaders/block_frag.glsl");
    font_shader = Shader("shaders/font_vert.glsl", "shaders/font_frag.glsl");
    gui_shader = Shader("shaders/gui_vert.glsl", "shaders/gui_frag.glsl");

    texture_atlas = Texture("textures/texture_atlas.png", 0);
    texture_atlas.use_mipmap("textures/texture_atlas_mipmap.png", 1);

    day_lightmap = Texture("textures/day_lightmap.png", 1);
    night_lightmap = Texture("textures/night_lightmap.png", 2);

    font = Texture("textures/font.png", 3);
    gui = Texture("textures/gui.png", 4);

    texture_atlas.bind();
    day_lightmap.bind();
    night_lightmap.bind();
    font.bind();
    gui.bind();

    block_shader.bind();
    block_shader.upload_vec4("fog_color", vec4(192, 216, 255, 255) / 255.0f);
    block_shader.upload_texture("tex0", texture_atlas);
    block_shader.upload_texture("tex1", day_lightmap);
    block_shader.upload_texture("tex2", night_lightmap);

    font_shader.bind();
    font_shader.upload_texture("tex3", font);
    font_shader.upload_int("win_width", win_size.x);
    font_shader.upload_int("win_height", win_size.y);

    gui_shader.bind();
    gui_shader.upload_int("win_width", win_size.x);
    gui_shader.upload_int("win_height", win_size.y);

    blocks_init();

    player = new Player(vec3(8.5, 122, 8.5));
    world = new World();

    debug_text = Text(ivec2(1920 * -0.5f + 6, 1080 * 0.5 - 6));
    crosshair = Sprite(vec2(-9 / 2 * 3 - 1, -9 / 2 * 3 - 2), vec2(9, 9), vec2(15 * 16 + 3, 3 + 9), &gui);

    toggle_mouse_control();
}

void Game::update()
{
    handle_misc_keys();

    world->update();
    player->update();

    if (show_inventory)
        player->inventory.update();

    if (show_debug)
    {
        std::stringstream buf;

        buf << "fps: " << (int)(1 / delta_time) << "\n\n";

        buf << "load_q: " << world->load_q.size() << "\n" << "dirty: " << world->dirty_chunks.size() << "\n\n";

        buf << "pos: " << std::fixed << std::setprecision(4) << player->pos.x << " / " << std::fixed << std::setprecision(4) << player->pos.y << " / " << std::fixed << std::setprecision(4) << player->pos.z << "\n";
        buf << "vel: " << std::fixed << std::setprecision(4) << player->vel.x << " / " << std::fixed << std::setprecision(4) << player->vel.y << " / " << std::fixed << std::setprecision(4) << player->vel.z << "   " << length(player->vel) << "\n\n";
        buf << "hand: " << player->hotbar.get_item() << " in slot " << player->hotbar.get_index() << "\n";

        buf << "mode: ";

        if (survival_mode)
            buf << "survival\n";
        else
            buf << "creative\n";

        Chunk* chunk = world->get_chunk_from_pos(player->pos);

        if (chunk != NULL)
        {
            buf << "chunk: " << chunk->pos.x << " " << chunk->pos.y << "\n";

            ivec3 lp = player->pos - chunk_pos_to_world_hp(chunk->pos);
            lp.y++;

            if (in_chunk(lp))
                buf << "light: " << chunk->get_sky_light(lp) << " sky " << chunk->get_block_light(lp) << " block\n";
        }

        if (player->hit_info.hit)
        {
            buf << "\nblock: " << player->hit_info.block_pos.x << " " << player->hit_info.block_pos.y << " " << player->hit_info.block_pos.z << " in " << player->hit_info.chunk->pos.x << " " << player->hit_info.chunk->pos.y;

            BlockState* bs = player->hit_info.chunk->get(player->hit_info.block_pos);

            buf << "\nid: " << (int)bs->id << "\ndata1: " << std::setfill('0') << std::setw(2) << std::hex << (int)bs->data1 << "\ndata2: " << std::setfill('0') << std::setw(2) << std::hex << (int)bs->data2;
        }

        debug_text.set_str(buf.str());
    }
}

void Game::draw()
{
    vec3 day_color = vec3(121, 166, 255) / 255.0f;
    vec3 night_color = vec3(10, 15, 25) / 255.0f;

    float t = global_light / 15.0f;
    vec3 sky_color = t * day_color + (1 - t) * night_color;

    glClearColor(sky_color.x, sky_color.y, sky_color.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world->draw();
    player->draw();

    player->hotbar.draw();

    if (show_inventory)
        player->inventory.draw();
    else
    {
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

        crosshair.draw();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if (show_debug)
        debug_text.draw();
}

void Game::destroy()
{
    world->destroy();
    player->destroy();

    debug_text.destroy();
    crosshair.destroy();

    line_shader.destroy();
    block_shader.destroy();
    font_shader.destroy();
    gui_shader.destroy();

    texture_atlas.destroy();
    day_lightmap.destroy();
    night_lightmap.destroy();
    font.destroy();
}