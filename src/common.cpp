#include "common.h"
#include <SDL2/SDL_image.h>
#include "imgui.h"

#define M_PI 3.14159265358979323846

int PointInCircle(SDL_Point point, SDL_Point center, int radius)
{
    int dx = point.x - center.x;
    int dy = point.y - center.y;
    return dx * dx + dy * dy <= radius * radius;
}

SDL_Texture* CreateTextTexture(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    surface = NULL;
    return texture;
}

SDL_Texture* OpenImageTexture(SDL_Renderer* ren, const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void DrawLineThickness(SDL_Renderer* ren, SDL_Point p1, SDL_Point p2, int thickness)
{
    if(thickness > 1) {
        for(int i = -thickness/2; i < thickness-thickness/2; i++) {
            SDL_RenderDrawLine(ren, p1.x, p1.y + i, p2.x, p2.y + i);
        }
    }
    else {
        SDL_RenderDrawLine(ren, p1.x, p1.y, p2.x, p2.y);
    }
}

void FillCircle(SDL_Renderer *ren, SDL_Point center, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(ren, center.x + dx, center.y + dy);
            }
        }
    }
}

void DrawCircle(SDL_Renderer *ren, SDL_Point center, int radius) {
    for (double i = 0; i < 2 * M_PI; i += 0.01) {
        SDL_RenderDrawPoint(ren, center.x + radius * cos(i), center.y + radius * sin(i));
    }
}

int int_log2(uint32_t x) { return 31 - __builtin_clz(x|1); }

int count_digits(uint32_t x) {
    static uint32_t table[] = {9, 99, 999, 9999, 99999, 
    999999, 9999999, 99999999, 999999999};
    int y = (9 * int_log2(x)) >> 5;
    y += x > table[y];
    return y + 1;
}

void info_box(char*** msgs, int* msgs_count) {
    static bool show;
    if(msgs == NULL) {
        return;
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Info", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        show = true;
        for(int i = 0; i < *msgs_count; i++) {
            ImGui::Text("%s", (*msgs)[i]);
        }
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            show = false;
        }
        ImGui::EndPopup();
    }
    if(*msgs != NULL && !show) {
        for(int i = 0; i < *msgs_count; i++) {
            free((*msgs)[i]);
        }
        free(*msgs);
        *msgs = NULL;
        *msgs_count = 0;
    }
};

void add_msg(const char* msg, char*** msgs, int* msgs_count) {
    *msgs = (char**)realloc(*msgs, sizeof(char*) * (*msgs_count + 1));
    (*msgs)[*msgs_count] = (char*)malloc(sizeof(char) * (strlen(msg) + 1));
    snprintf((*msgs)[*msgs_count], strlen(msg) + 1, "%s", msg);
    (*msgs_count)++;
};
