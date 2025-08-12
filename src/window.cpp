#include "window.h"

#include <SDL3/SDL.h>
#include "stateman.h"

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
                "ERROR: Failed to set vsync! SDL_Error: %s\n", SDL_GetError());
    };

    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

std::optional<std::pair<int, int>> Window::_getWindowSize() {
    std::pair<int, int> ret;
    if (!SDL_GetWindowSizeInPixels(mWindow, &ret.first, &ret.second)) return std::nullopt;
    return ret;
}

void Window::_renderWindow(State *s) {
    SDL_SetRenderDrawColor(mRenderer, 0x1e, 0x1e, 0x2e, 255);
    SDL_RenderClear(mRenderer);

    if (s->mTextureLoaded) s->renderTexture();

    SDL_RenderPresent(mRenderer);
}

static inline void handle_event(Window *w, SDL_Event *ev, StateManager *sm) {
    // NOTE: Use this to get no delay with the event.
    const bool *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_ESCAPE]) {
        w->mExit = true;
        return;
    }

    static const int zoomIncrement = 10;
    static const int movementIncrement = (int)(12 * (sm->mActive->mZoom / 100));
    if (key_state[SDL_SCANCODE_H])
        sm->mActive->moveTexturePosBy(std::pair<int, int>(movementIncrement, 0));
    if (key_state[SDL_SCANCODE_J])
        sm->mActive->moveTexturePosBy(std::pair<int, int>(0, -movementIncrement));
    if (key_state[SDL_SCANCODE_K])
        sm->mActive->moveTexturePosBy(std::pair<int, int>(0, movementIncrement));
    if (key_state[SDL_SCANCODE_L])
        sm->mActive->moveTexturePosBy(std::pair<int, int>(-movementIncrement, 0));

    while (SDL_PollEvent(ev)) {
        switch (ev->type) {
            case SDL_EVENT_QUIT:
                w->mExit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                // NOTE: Use this if you want a delay with that key.
                switch (ev->key.key) {
                    case SDLK_MINUS:
                        sm->mActive->zoomTextureBy(-zoomIncrement);
                        break;
                    case SDLK_EQUALS:
                        sm->mActive->zoomTextureBy(zoomIncrement);
                        break;
                    case SDLK_N:
                        {
                              size_t next = sm->mActiveIdx + 1;
                              sm->activeteState(next);
                              break;
                        }
                    case SDLK_P:
                        {
                              size_t next = sm->mActiveIdx - 1;
                              sm->activeteState(next);
                              break;
                        }
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

bool Window::startWindowLoops(StateManager *sm) {
    SDL_Event event;

    while (!mExit) {
        _renderWindow(sm->mActive);
        handle_event(this, &event, sm);
    }

    return true;
}
