#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <optional>

struct StateManager;
struct State;

class Window {
    public:
        SDL_Renderer *mRenderer;
        SDL_Window *mWindow;
        size_t mWidth, mHeight, mFPS;
        const char *mTitle;
        bool mExit;

        bool mCommandMode;
        std::string mCommandBuff;

        Window(size_t w, size_t h, size_t fps, const char *name);
        ~Window();

        std::optional<std::string> initWindow();
        std::optional<std::string> resizeWindow(size_t newWidth, size_t newHeight);
        std::pair<int, int> getWindowSize(std::string *err);
        bool startWindowLoop(StateManager *s);
    private:
        void _renderWindow(State *s);
};
