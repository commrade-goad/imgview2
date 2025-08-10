#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "state.h"
#include "stb_image.h"

#include <iostream>

State::State(char *path) { _stateInit(path, SDL_SCALEMODE_NEAREST); }
State::State(char *path, SDL_ScaleMode scaleMode) { _stateInit(path, scaleMode); }

std::optional<std::string> State::setScaleMode(SDL_ScaleMode mode) {
    std::string result;
    result.resize(512);

    if (mTexture) {
        if (!SDL_SetTextureScaleMode(mTexture, mode)) {
            std::snprintf(result.data(), result.size(),
                    "ERROR: Failed to set texture scaling mode! SDL_Error: %s\n", SDL_GetError());
        }
    } else {
        std::snprintf(result.data(), result.size(),
                "ERROR: Texture is not initialized!");
    }

    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

State::~State() {}

std::optional<std::string> State::_loadImage() {
    std::string result;
    result.resize(512);

    int x, y, n;
    unsigned char *data = stbi_load(mPath, &x, &y, &n, 0);
    if (!data) {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to load the image: %s\n", stbi_failure_reason());
        return result;
    }

    stbi_image_free(data);
    if (strlen(result.data()) > 0) return result;
    else                           return std::nullopt;
}

void State::_stateInit(const char *path, SDL_ScaleMode mode) {
    mPath = path;
    mPos = std::pair<int, int>(0,0);
    mZoom = 100;
    mTexture = nullptr;
    mScaleMode = mode;

    auto result = _loadImage();
    if (result.has_value())
        std::cerr << result.value() << std::endl;
}
