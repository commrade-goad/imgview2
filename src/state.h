#pragma once

#include <utility>
#include <string>
#include <optional>
#include <SDL3/SDL.h>
#include <mutex>

#include "image.h"

struct Window;

class State {
    public:
        Window *mWindow;
        const char *mPath;
        SDL_FRect mRec;
        int mZoom;
        SDL_Texture *mTexture;
        SDL_ScaleMode mScaleMode;
        int mError;
        bool mTextureLoaded;
        bool mFitIn;

        // NOTE: Access using mutex!
        std::mutex mMutex;
        Image mImageData;

        State(Window *win, const char *path, SDL_ScaleMode scaleMode);
        State(Window *win, const char *path);
        ~State();

        std::optional<std::string> setScaleMode(SDL_ScaleMode mode);
        void renderTexture();
        std::optional<std::string> loadEverythingSync();
        std::optional<std::string> loadImage();
        std::optional<std::string> createTexture();

        void moveTexturePosBy(std::pair<int, int> n);
        void zoomTextureBy(int n);
        void fitTextureToWindow();

    private:
        void _stateInit(Window *win, const char *path, SDL_ScaleMode mode);
        void _regenerateRec();
};
