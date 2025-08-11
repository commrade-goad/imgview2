#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <optional>

class Window {
    public:
        SDL_Renderer *mRenderer;
        SDL_Window *mWindow;
        size_t mWidth, mHeight, mFPS;
        const char *mTitle;
        bool mExit;

        Window(size_t w, size_t h, size_t fps, const char *name);
        ~Window();

        std::optional<std::string> initWindow();
        std::optional<std::string> resizeWindow(size_t newWidth, size_t newHeight);
        void startWindowLoops();
    private:
        size_t dt;
        Uint64 fps_delay;

        std::optional<std::pair<int, int>> _getWindowSize();
        void _renderWindow();
};
