#pragma once

#include <utility>
#include <string>
#include <optional>
#include <SDL3/SDL.h>

class State {
    public:
        const char *mPath;
        std::pair<int, int>mPos;
        int mZoom;
        SDL_Texture *mTexture;
        SDL_ScaleMode mScaleMode;

        State(char *path, SDL_ScaleMode scaleMode);
        State(char *path);
        ~State();

        std::optional<std::string> setScaleMode(SDL_ScaleMode mode);
    private:
        std::optional<std::string> _loadImage();
        void _stateInit(const char *path, SDL_ScaleMode mode);
};
