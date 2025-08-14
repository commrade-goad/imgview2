#include "window.h"

#include <SDL3/SDL.h>
#include "stateman.h"
#include "event.h"

Window::Window(size_t w, size_t h, size_t fps, const char *name) {
    mWidth = w;
    mHeight = h;
    mTitle = name;
    mFPS = fps;
    mRenderer = nullptr;
    mWindow = nullptr;
    mExit = false;
    // mCommandMode = false;
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
                "ERROR: Failed to set vsync! SDL_Error: %s\n", SDL_GetError());
    };

    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

std::pair<int, int> Window::getWindowSize(std::string *err) {
    std::pair<int, int> ret;
    if (!SDL_GetWindowSizeInPixels(mWindow, &ret.first, &ret.second)) {
        err->resize(512);
        snprintf(
                err->data(), err->size(),
                "ERROR: Failed to get the window size! SDL Error: %s\n", SDL_GetError()
                );
        return std::pair<int, int>();
    }
    return ret;
}

void Window::_renderWindow(State *s) {
    SDL_SetRenderDrawColor(mRenderer, 0x1e, 0x1e, 0x2e, 255);
    SDL_RenderClear(mRenderer);

    s->renderTexture();

    SDL_RenderPresent(mRenderer);
}

bool Window::startWindowLoop(StateManager *sm) {
    SDL_Event event;

    Uint64 FPSDelay = 1000 / mFPS;

    while (!mExit) {

        Uint64 frameStart = SDL_GetTicks();

        _renderWindow(sm->mActive);
        handleEvent(this, &event, sm);

        Uint64 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FPSDelay) {
            SDL_Delay(FPSDelay - frameTime);
        }
    }

    return true;
}
