#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "titlebar.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <string>

class Window {
private:
    SDL_Window *win_;
    SDL_Renderer *ren_;
    TTF_Font *font_;
    Titlebar* titlebar_;
    SDL_Color color_;
    ImGuiContext* context_;

public:
    Window(int argc, char* argv[], SDL_Point size, const char* title, SDL_Color color, const char* ttf_path);
    ~Window();
    int events(SDL_Event *e, int *quit, char*** pmsgs, int* pmsgs_count, bool* subwindows_active);
    void clear();
    void titlebar_draw();
    void render(int fps);
    void show_msgs(char*** msgs, int* msgs_count);

    SDL_Window* win() { return win_; }
    SDL_Renderer* ren() { return ren_; }
    Titlebar* titlebar() { return titlebar_; }
    SDL_Color color() { return color_; }
    ImGuiContext* context() { return context_; }
    TTF_Font* font() { return font_; }
};

#endif
