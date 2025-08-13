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

    if (mTexture && mTextureLoaded) {
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
    else {
        mTextureLoaded = true;
        _regenerateRec();
        if (mFitIn) fitTextureToWindow();
        return std::nullopt;
    }
}
void State::_regenerateRec() {
    mRec = SDL_FRect {
        (float)(mRec.x),
        (float)(mRec.y),
        (float)(mTexture->w) * (mZoom / 100.0f),
        (float)(mTexture->h) * (mZoom / 100.0f)
    };
}

void State::_stateInit(Window *win, const char *path, SDL_ScaleMode mode) {
    mPath = path;
    mZoom = 100;
    mTexture = nullptr;
    mScaleMode = mode;
    mWindow = win;
    mError = 0;
    mTextureLoaded = false;
    mImageData = {};
    mRec = {};
    mFitIn = true;
}

void State::renderTexture(){
    // NOTE: Not loaded atleast try creating the texture.
    if (!mTexture && !mTextureLoaded) {
        if (mImageData.data) createTexture();
        else return;
    }

    if (!SDL_RenderTexture(mWindow->mRenderer, mTexture, NULL, &mRec))
        std::cerr << "ERROR: Failed to render : " << SDL_GetError() << std::endl;
}

std::optional<std::string> State::loadEverythingSync() {
    if (mTextureLoaded && mTexture) return std::nullopt;

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
    mRec.x += n.first;
    mRec.y += n.second;
    _regenerateRec();
    mFitIn = false;
}

void State::zoomTextureBy(int n) {
    mZoom += n;
    _regenerateRec();
    mFitIn = false;
}

void State::fitTextureToWindow() {
    if (!mWindow) return;

    std::string err;
    std::pair<int, int> windowSize = mWindow->getWindowSize(&err);
    if (err.size() > 0) return;
    
    if (!mTexture && !mTextureLoaded) return;

    double windowAspect = (double)windowSize.first / windowSize.second;
    double imageAspect  = (double)mTexture->w / mTexture->h;

    if (windowAspect < imageAspect) {
        mRec.w = windowSize.first;
        mRec.h = round(windowSize.first / imageAspect);
    } else {
        mRec.w = round(windowSize.second * imageAspect);
        mRec.h = windowSize.second;
    }

    mRec.x = round((windowSize.first - mRec.w) / 2);
    mRec.y = round((windowSize.second - mRec.h) / 2);

    mZoom = round((mRec.w / mTexture->w) * 100);
    mFitIn = true;
}
