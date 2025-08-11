#define STB_IMAGE_IMPLEMENTATION

#include "state.h"
#include "window.h"
#include "stb_image.h"

#include <iostream>

State::State(Window *w, const char *path) { _stateInit(w, path, SDL_SCALEMODE_NEAREST); }
State::State(Window *w, const char *path, SDL_ScaleMode scaleMode) { _stateInit(w, path, scaleMode); }

std::optional<std::string> State::setScaleMode(SDL_ScaleMode mode) {
    std::string result;
    result.resize(512);

    if (mTexture) {
        if (!SDL_SetTextureScaleMode(mTexture, mode)) {
            std::snprintf(result.data(), result.size(),
                    "ERROR: Failed to set texture scaling mode! SDL_Error: %s\n", SDL_GetError());
            mError++;
        }
    } else {
        std::snprintf(result.data(), result.size(),
                "ERROR: Texture is not initialized!");
        mError++;
    }

    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

State::~State() { SDL_DestroyTexture(mTexture); }

std::optional<std::string> State::_loadImage() {
    return std::nullopt;
    std::string result;
    result.resize(512);

    int x, y, n;
    unsigned char *data = stbi_load(mPath, &x, &y, &n, 4);
    if (!data) {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to load the image: %s\n", stbi_failure_reason());
        mError++;
        return result;
    }

    SDL_Surface *surface =
        SDL_CreateSurfaceFrom(x, y, SDL_PIXELFORMAT_BGRA32,
                data, x * 4);

    if (!surface) {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to create the texture from image: %s\n", SDL_GetError());
        stbi_image_free(data);
        mError++;
        return result;
    }

    mTexture = SDL_CreateTextureFromSurface(mWindow->mRenderer, surface);

    stbi_image_free(data);
    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

void State::_stateInit(Window *win, const char *path, SDL_ScaleMode mode) {
    mPath = path;
    mPos = std::pair<int, int>(0,0);
    mZoom = 100;
    mTexture = nullptr;
    mScaleMode = mode;
    mWindow = win;
    mError = 0;

    auto result = _loadImage();
    if (result.has_value())
        std::cerr << result.value();
}
