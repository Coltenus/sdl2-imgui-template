#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <random>

int PointInCircle(SDL_Point point, SDL_Point center, int radius);
SDL_Texture* CreateTextTexture(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color);
SDL_Texture* OpenImageTexture(SDL_Renderer* ren, const char* path);
void DrawLineThickness(SDL_Renderer* ren, SDL_Point p1, SDL_Point p2, int thickness);
void FillCircle(SDL_Renderer* ren, SDL_Point center, int radius);
void DrawCircle(SDL_Renderer* ren, SDL_Point center, int radius);
int int_log2(uint32_t x);
int count_digits(uint32_t x);
void info_box(char*** msgs, int* msgs_count);
void add_msg(const char* msg, char*** msgs, int* msgs_count);

template <typename T>
T random_value(T min, T max) {
    std::random_device rd;
    std::mt19937 gen(rd());

    if constexpr (std::is_integral<T>::value) {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(gen);
    } else if constexpr (std::is_floating_point<T>::value) {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(gen);
    } else {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, 
                      "T must be an integral or floating-point type");
    }
}

#endif
