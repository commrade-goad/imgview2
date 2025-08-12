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

std::optional<std::string> State::loadImage() {
    std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
    if (!lock) return std::nullopt;

    std::string result;
    result.resize(512);

    unsigned char *data = stbi_load(mPath, &mImageData.x, &mImageData.y, &mImageData.n, 4);
    if (!data) {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to load the image: %s\n", stbi_failure_reason());
        mError++;
        return result;
    }
    mImageData.data = data;
    return std::nullopt;
}

std::optional<std::string> State::createTexture() {
    std::unique_lock<std::mutex> lock(mMutex, std::try_to_lock);
    if (!lock) return std::nullopt;

    std::string result;
    result.resize(512);

    if (!mImageData.data) {
        snprintf(result.data(), result.size(),
                 "ERROR: Image is not loaded! Can't make a texture from void!\n");
        stbi_image_free(mImageData.data);
        return result;
    }

    SDL_Surface *surface =
        SDL_CreateSurfaceFrom(mImageData.x, mImageData.y, SDL_PIXELFORMAT_RGBA32,
                mImageData.data, mImageData.x * 4);

    if (!surface) {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to create the surface from image: %s\n", SDL_GetError());
        stbi_image_free(mImageData.data);
        mError++;
        return result;
    }

    SDL_Texture *text = SDL_CreateTextureFromSurface(mWindow->mRenderer, surface);
    if (text) mTexture = text;
    else {
        snprintf(result.data(), result.size(),
                 "ERROR: Failed to create the texture from image: %s\n", SDL_GetError());
        mError++;
    }

    SDL_DestroySurface(surface);
    stbi_image_free(mImageData.data);
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
    mTextureLoaded = false;
    mImageData = {};
}

void State::renderTexture(){
    if (!mTexture) return;
    const SDL_FRect rec =
        SDL_FRect {
            static_cast<float>(mPos.first),
            static_cast<float>(mPos.second),
            static_cast<float>(mTexture->w) * (mZoom / 100.0f),
            static_cast<float>(mTexture->h) * (mZoom / 100.0f)
        };
    if (!SDL_RenderTexture(mWindow->mRenderer, mTexture, NULL, &rec))
        std::cerr << "ERROR: Failed to render : " << SDL_GetError() << std::endl;
}

std::optional<std::string> State::loadEverythingSync() {
    if (mTextureLoaded) return std::nullopt;

    auto result = loadImage();
    if (result.has_value())
        return result.value();

    result = createTexture();
    if (result.has_value())
        return result.value();

    mTextureLoaded = true;
    return std::nullopt;
}

void State::moveTexturePosBy(std::pair<int, int> n) {
    mPos.first += n.first;
    mPos.second += n.second;
}

void State::zoomTextureBy(int n) { mZoom += n; }
