#include "window.h"

#include <SDL3/SDL.h>

Window::Window(size_t w, size_t h, size_t fps, const char *name) {
    mWidth = w;
    mHeight = h;
    mTitle = name;
    mFPS = fps;
    mRenderer = nullptr;
    mWindow = nullptr;
    mExit = false;
}

Window::~Window() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

std::optional<std::string> Window::initWindow() {
    std::string result;
    result.resize(512);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::snprintf(result.data(), result.size(),
                "ERROR: SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(mTitle, mWidth, mHeight,
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN,
                &mWindow, &mRenderer))
    {
        std::snprintf(result.data(), result.size(),
                "ERROR: Window and renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    }

    if (!SDL_SetRenderVSync(mRenderer, 1))
    {
        std::snprintf(result.data(), result.size(),
                "ERROR: Failed to set vsunc! SDL_Error: %s\n", SDL_GetError());
    };

    if (result.size() > 0) return result;
    else                   return std::nullopt;
}

std::optional<std::pair<int, int>> Window::_getWindowSize() {
    std::pair<int, int> ret;
    if (!SDL_GetWindowSizeInPixels(mWindow, &ret.first, &ret.second)) return std::nullopt;
    return ret;
}

void Window::_renderWindow() {
    SDL_SetRenderDrawColor(mRenderer, 0x1e, 0x1e, 0x2e, 255);
    SDL_RenderClear(mRenderer);
    SDL_RenderPresent(mRenderer);
}

void Window::startWindowLoops() {
    SDL_Event event;

    Uint64 current_time = SDL_GetTicksNS();
    Uint64 future_time = SDL_GetTicksNS();

    while (!mExit) {
        Uint64 frame_start = SDL_GetTicks();

        future_time = SDL_GetTicksNS();
        dt = future_time - current_time / 1000000;
        current_time = future_time;

        _renderWindow();
        _handleWindowEvent(&event);

        Uint64 frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < fps_delay) {
            SDL_Delay(fps_delay - frame_time);
        }
    }
}

void Window::_handleWindowEvent(SDL_Event *event) {
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_EVENT_QUIT:
                mExit = true;
                return;
            default:
                return;
        }
    }
}
