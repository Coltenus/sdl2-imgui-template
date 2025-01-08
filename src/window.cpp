#include "window.h"
#include <SDL2/SDL_image.h>
#include "common.h"
#include <GL/glew.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#include <SDL2/SDL_syswm.h>
#if defined(_WIN32)
#include <shellapi.h>
#endif
#include <time.h>
#include <fstream>
#include <sstream>

Window::Window(int argc, char* argv[], SDL_Point size, const char* title, SDL_Color color, const char* ttf_path)
{
    char path[256];
    win_ = NULL;
    ren_ = NULL;
    color_ = color;

    if(argc > 1 && strncmp(argv[1], "--log", 5) == 0) {
        snprintf(path, sizeof(path), "%s/log.txt", SDL_GetBasePath());
        freopen(path, "w", stdout);
        printf("Log file opened\n");
    }
    else {
        printf("Log file not opened\n");
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    win_ = SDL_CreateWindow(title,
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

    if (win_ == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    int oglIdx = -1;
    int nRD = SDL_GetNumRenderDrivers();
    for(int i=0; i<nRD; i++)
    {
        SDL_RendererInfo info;
        if(!SDL_GetRenderDriverInfo(i, &info))
        {
            if(!strcmp(info.name, "opengl"))
            {
                oglIdx = i;
            }
        }
    }
    ren_ = SDL_CreateRenderer(win_, oglIdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren_ == NULL) {
        SDL_DestroyWindow(win_);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    SDL_SetRenderDrawBlendMode(ren_, SDL_BLENDMODE_BLEND);

    #if defined(_WIN32)
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    NOTIFYICONDATA icon;
    if (SDL_GetWindowWMInfo(win_, &info))
    {
        icon.uCallbackMessage = WM_USER + 1;
        icon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
        icon.hIcon = LoadIcon(NULL, IDI_INFORMATION);
        icon.cbSize = sizeof(icon);
        icon.hWnd = info.info.win.window;
        strcpy(icon.szTip, title);

        Shell_NotifyIcon(NIM_ADD, &icon);
    }
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    context_ = ImGui::CreateContext();
    ImGui::SetCurrentContext(context_);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    ImPlot::CreateContext();

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(win_, context_);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glewInit();

    if (TTF_Init() != 0) {
        SDL_DestroyRenderer(ren_);
        SDL_DestroyWindow(win_);
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    snprintf(path, sizeof(path), "%s%s", SDL_GetBasePath(), ttf_path);
    font_ = TTF_OpenFont(path, 60);
    if (font_ == NULL) {
        TTF_Quit();
        SDL_DestroyRenderer(ren_);
        SDL_DestroyWindow(win_);
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        TTF_CloseFont(font_);
        TTF_Quit();
        SDL_DestroyRenderer(ren_);
        SDL_DestroyWindow(win_);
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    titlebar_ = new Titlebar(ren_, win_, title, 30, {60, 60, 60, 255}, font_);
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext(context_);
    delete titlebar_;
    SDL_DestroyRenderer(ren_);
    SDL_DestroyWindow(win_);
    SDL_Quit();
    fclose(stdout);
}

int Window::events(SDL_Event *e, int *quit, char*** pmsgs, int* pmsgs_count, bool* subwindows_active)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL2_ProcessEvent(e);
    if(io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return 1;
    }
    if (e->type == SDL_QUIT) {
        *quit = 1;
        return 1;
    }
    else if(titlebar_->events(e)) {
        if(titlebar_->event_type() == 3 || titlebar_->event_type() == 4) {
            if(*pmsgs != NULL) {
                for(int i = 0; i < *pmsgs_count; i++) {
                    free((*pmsgs)[i]);
                }
                free(*pmsgs);
                *pmsgs = NULL;
                *pmsgs_count = 0;
            }
        }
        switch (titlebar_->event_type()) {
            case 1:
                *quit = 1;
                return 1;
            case 2:
                SDL_MinimizeWindow(win_);
                return 1;
            case 3:
                if(subwindows_active)
                    *subwindows_active = !*subwindows_active;
                return 1;
        }
        return 1;
    }
    return 0;
}

void Window::clear()
{
    static SDL_Rect vp;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    vp.x = vp.y = 0;
    vp.w = (int) ImGui::GetIO ().DisplaySize.x;
    vp.h = (int) ImGui::GetIO ().DisplaySize.y;
    SDL_RenderSetViewport (ren_, &vp);
    SDL_SetRenderDrawColor(ren_, color_.r, color_.g, color_.b, color_.a);
    SDL_RenderClear(ren_);
}

void Window::titlebar_draw()
{
    titlebar_->draw();
}

void Window::render(int fps)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(ren_);
    SDL_Delay(1000 / fps);
}

void Window::show_msgs(char ***msgs, int *msgs_count)
{
    if(*msgs_count != 0) {
        ImGui::OpenPopup("Info");
    }
    info_box(msgs, msgs_count);
}
