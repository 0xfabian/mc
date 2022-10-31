#include <global.h>
#include <game.h>

void handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            closed = true;
            break;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            int index = event.key.keysym.sym & (~0x40000000);

            if (index < KEY_MAX && keys[index] == Input::NONE)
                keys[index] = Input::DOWN;
        }
        else if (event.type == SDL_KEYUP)
        {
            int index = event.key.keysym.sym & (~0x40000000);

            if (index < KEY_MAX)
                keys[index] = Input::UP;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int index = event.button.button - 1;

            if (index < BUTTON_MAX && buttons[index] == Input::NONE)
                buttons[index] = Input::DOWN;
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            int index = event.button.button - 1;

            if (index < BUTTON_MAX)
                buttons[index] = Input::UP;
        }
    }
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    win = SDL_CreateWindow
    (
        win_name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_size.x,
        win_size.y,
        SDL_WINDOW_OPENGL | (is_fullscreen ? SDL_WINDOW_FULLSCREEN : 0)
    );

    SDL_GLContext context = SDL_GL_CreateContext(win);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    Game::init();

    Uint64 this_frame = SDL_GetPerformanceCounter();
    Uint64 last_frame = 0;

    while (!closed)
    {
        last_frame = this_frame;
        this_frame = SDL_GetPerformanceCounter();

        delta_time = (this_frame - last_frame) / (double)SDL_GetPerformanceFrequency();

        update_inputs();
        handle_events();

        Game::update();
        Game::draw();

        SDL_GL_SwapWindow(win);
    }

    Game::destroy();

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}