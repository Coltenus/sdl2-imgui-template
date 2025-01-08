#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "window.h"
#include <string>
#include <fmt/format.h>
#include "common.h"

int main(int argc, char* argv[]) {
    SDL_Point size = {1000, 600};
    Window* window = new Window(argc, argv, size, "SDL2 ImGui Template", {130, 90, 160, 255}, "assets/font.ttf");

    SDL_Event e;
    int quit = 0;
    char **msgs = NULL;
    int msgs_count = 0;
    char **msgs_box = NULL;
    int msgs_count_box = 0;
    ImVec4 inactive_color = ImVec4(39.0f / 255.0f, 72.0f / 255.0f, 114.0f / 255.0f, 1.0f);
    ImVec4 active_color = ImVec4(66.f / 255.f, 150.f / 255.f, 250.f / 255.f, 1.0f);
    bool subwindows_active = true;
    bool test_state = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if(window->events(&e, &quit, &msgs_box, &msgs_count_box, &subwindows_active)) {
                continue;
            }
            if(e.type == SDL_KEYUP) {
                if(e.key.keysym.sym == SDLK_p) {
                    SDL_MessageBoxButtonData buttons[] = {
                        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"},
                        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Cancel"},
                    };
                    SDL_MessageBoxData data = {
                        SDL_MESSAGEBOX_INFORMATION,
                        window->win(),
                        "Info",
                        "Escape key pressed",
                        SDL_arraysize(buttons),
                        buttons,
                        NULL
                    };
                    int buttonid;
                    SDL_ShowMessageBox(&data, &buttonid);
                    printf("Button pressed: %d\n", buttonid);
                }
            }
        }

        window->clear();

        if(subwindows_active && !(SDL_GetWindowFlags(window->win()) & SDL_WINDOW_MINIMIZED)) {
            if(msgs_count_box > 0) {
                ImGui::Begin("Info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
                for(int i = 0; i < msgs_count_box; i++) {
                    ImGui::Text("%s", msgs_box[i]);
                }
                if(ImGui::Button("Close")) {
                    for(int i = 0; i < msgs_count_box; i++) {
                        free(msgs_box[i]);
                    }
                    free(msgs_box);
                    msgs_box = NULL;
                    msgs_count_box = 0;
                }
                ImGui::End();
            }

            {
                ImGui::SetNextWindowPos({10, (float)window->titlebar()->size().y + 10});
                ImGui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
                | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
                ImGui::PushStyleColor(ImGuiCol_Button, test_state ? active_color : inactive_color);
                if(ImGui::Button("Test")) {
                    test_state = !test_state;
                    add_msg("Test", &msgs, &msgs_count);
                }
                ImGui::PopStyleColor();
                ImGui::End();
            }

            {
                int data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
                float data2[10] = {0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5};
                ImGui::Begin("Nodes");
                if(ImPlot::BeginPlot("Nodes")) {
                    ImPlot::PlotBars("Nodes", data, 10);
                    ImPlot::PlotLine("Nodes", data2, 10);
                    ImPlot::EndPlot();
                }
                ImGui::End();
            }

            window->show_msgs(&msgs, &msgs_count);
        }
        
        window->titlebar_draw();

        window->render(60);
    }

    delete window;

    return 0;
}
