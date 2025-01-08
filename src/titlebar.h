#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "button.h"
#include <vector>

class Titlebar {
private:
    SDL_Point size_;
    SDL_Color color;
    SDL_Texture* texture;
    SDL_Rect text_rect;
    SDL_Renderer* ren;
    std::vector<Button*> buttons;
    int event_type_, dragging;
    SDL_Point offset;
    
public:
    Titlebar(SDL_Renderer* ren, SDL_Window* win, const char* title, int height, SDL_Color color, TTF_Font* font);
    ~Titlebar();
    void draw();
    int events(SDL_Event* e);

    SDL_Point size() { return size_; }
    int event_type() { return event_type_; }
};

#endif
