#ifndef WINDOWS_GAMES_COMPAT_WINBGIM_H
#define WINDOWS_GAMES_COMPAT_WINBGIM_H

#include <queue>

#include "../SDL2_bgi-3.0.4/src/graphics.h"

#ifdef initwindow
#undef initwindow
#endif
#ifdef ismouseclick
#undef ismouseclick
#endif
#ifdef getmouseclick
#undef getmouseclick
#endif
#ifdef delay
#undef delay
#endif
#ifdef getch
#undef getch
#endif
#ifdef kbhit
#undef kbhit
#endif
inline int windows_games_initwindow(int width, int height)
{
    int result = _initwin_1(width, height);
    sdlbgislow();
    return result;
}

inline int windows_games_initwindow(int width, int height, char *title)
{
    int result = _initwin_2(width, height, title);
    sdlbgislow();
    return result;
}

struct WindowsGamesMouseClick
{
    int kind;
    int x;
    int y;
    bool pending;
};

inline WindowsGamesMouseClick &windows_games_mouse_click()
{
    static WindowsGamesMouseClick click = {0, -1, -1, false};
    return click;
}

inline std::queue<int> &windows_games_key_queue()
{
    static std::queue<int> keys;
    return keys;
}

inline bool windows_games_is_ignored_key(SDL_Keycode key)
{
    return key == SDLK_LCTRL || key == SDLK_RCTRL ||
           key == SDLK_LSHIFT || key == SDLK_RSHIFT ||
           key == SDLK_LGUI || key == SDLK_RGUI ||
           key == SDLK_LALT || key == SDLK_RALT ||
           key == SDLK_CAPSLOCK || key == SDLK_MENU ||
           key == SDLK_APPLICATION;
}

inline int windows_games_translate_key(const SDL_KeyboardEvent &key_event)
{
    SDL_Keycode key = key_event.keysym.sym;
    if (windows_games_is_ignored_key(key))
        return 0;

    SDL_Keymod keymod = SDL_GetModState();
    if ((keymod & (KMOD_LSHIFT | KMOD_RSHIFT | KMOD_CAPS)) && key >= 'a' && key <= 'z')
        key -= ('a' - 'A');

    return static_cast<int>(key);
}

inline void windows_games_store_mouse_click(const SDL_MouseButtonEvent &button)
{
    WindowsGamesMouseClick &click = windows_games_mouse_click();
    if (click.pending)
        return;

    float logical_x = 0.0f;
    float logical_y = 0.0f;
    SDL_RenderWindowToLogical(bgi_renderer, button.x, button.y, &logical_x, &logical_y);

    click.kind = button.button;
    click.x = static_cast<int>(logical_x);
    click.y = static_cast<int>(logical_y);
    click.pending = true;
}

inline void windows_games_collect_input()
{
    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            windows_games_store_mouse_click(event.button);
        } else if (event.type == SDL_KEYDOWN) {
            int key = windows_games_translate_key(event.key);
            if (key != 0)
                windows_games_key_queue().push(key);
        } else if (event.type == SDL_QUIT) {
            windows_games_key_queue().push(QUIT);
        }
    }
}

inline int windows_games_ismouseclick(int kind)
{
    windows_games_collect_input();

    WindowsGamesMouseClick &click = windows_games_mouse_click();
    return click.pending && click.kind == kind;
}

inline void windows_games_getmouseclick(int kind, int &x, int &y)
{
    windows_games_collect_input();

    WindowsGamesMouseClick &click = windows_games_mouse_click();
    if (click.pending && click.kind == kind) {
        x = click.x;
        y = click.y;
        click.pending = false;
    } else {
        x = -1;
        y = -1;
    }
}

inline int windows_games_kbhit()
{
    refresh();
    windows_games_collect_input();
    return !windows_games_key_queue().empty();
}

inline int windows_games_getch()
{
    refresh();

    while (windows_games_key_queue().empty()) {
        windows_games_collect_input();
        if (windows_games_key_queue().empty())
            SDL_Delay(1);
    }

    int key = windows_games_key_queue().front();
    windows_games_key_queue().pop();
    return key;
}

inline void windows_games_delay(int milliseconds)
{
    refresh();

    Uint32 stop = SDL_GetTicks() + static_cast<Uint32>(milliseconds);
    while (SDL_GetTicks() < stop) {
        windows_games_collect_input();
        SDL_Delay(1);
    }

    refresh();
}

#define initwindow(...) windows_games_initwindow(__VA_ARGS__)
#define ismouseclick(kind) windows_games_ismouseclick(kind)
#define getmouseclick(kind, x, y) windows_games_getmouseclick((kind), (x), (y))
#define kbhit() windows_games_kbhit()
#define getch() windows_games_getch()
#define delay(milliseconds) windows_games_delay(milliseconds)

#endif
