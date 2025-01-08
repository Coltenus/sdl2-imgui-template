#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Button {
private:
    SDL_Rect rect_, text_rect;
    SDL_Color color;
    SDL_Texture* texture;
    SDL_Renderer* ren;

public:
    Button(SDL_Renderer* ren, SDL_Point pos, int height, SDL_Color color, const char* text, TTF_Font* font);
    Button(SDL_Renderer* ren, SDL_Rect rect, SDL_Texture* texture, SDL_Color color);
    ~Button();
    void draw(int* selected, int value);
    int events(SDL_Event* e);
    void set_width(int width);
    void set_gap(int gap);

    SDL_Rect rect() { return rect_; }
};

#endif
