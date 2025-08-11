#pragma once

#include <utility>
#include <string>
#include <optional>
#include <SDL3/SDL.h>

struct Window;

class State {
    public:
        Window *mWindow;
        const char *mPath;
        std::pair<int, int>mPos;
        int mZoom;
        SDL_Texture *mTexture;
        SDL_ScaleMode mScaleMode;
        int mError;

        State(Window *win, const char *path, SDL_ScaleMode scaleMode);
        State(Window *win, const char *path);
        ~State();

        std::optional<std::string> setScaleMode(SDL_ScaleMode mode);
        void renderImage();

    private:
        std::optional<std::string> _loadImage();
        void _stateInit(Window *win, const char *path, SDL_ScaleMode mode);
};
