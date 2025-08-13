#pragma once

#include <iostream>
#include "stateman.h"
#include "window.h"

#define ZOOM_INCREMENT 5
#define MOVEMENT_INCREMENT 12

static inline void handleKeyCommand(Window *w, StateManager *sm, SDL_Event *ev) {
    switch (ev->key.key) {
        case SDLK_SEMICOLON:
            {
                std::cerr << "Exiting command mode..." << std::endl;
                w->mCommandMode = false;
                w->mCommandBuff.clear();
                break;
            }
        default:
            break;
    }
}

static inline void handleKeyNormal(Window *w, StateManager *sm, SDL_Event *ev) {
    switch (ev->key.key) {
        case SDLK_MINUS:
            sm->mActive->zoomTextureBy(-ZOOM_INCREMENT);
            break;
        case SDLK_EQUALS:
            sm->mActive->zoomTextureBy(ZOOM_INCREMENT);
            break;
        case SDLK_R:
            sm->mActive->fitTextureToWindow();
            break;
        case SDLK_S:
            {
                SDL_ScaleMode scaleMode =
                    sm->mActive->mScaleMode == SDL_SCALEMODE_LINEAR
                    ? SDL_SCALEMODE_NEAREST
                    : SDL_SCALEMODE_LINEAR;
                auto result = sm->mActive->setScaleMode(scaleMode);
                if (result.has_value()) std::cerr << result.value();
                break;
            }
        case SDLK_N:
            {
                size_t next = sm->mActiveIdx + 1;
                sm->activateState(next);
                break;
            }
        case SDLK_P:
            {
                size_t next = sm->mActiveIdx - 1;
                sm->activateState(next);
                break;
            }
        case SDLK_SEMICOLON:
            {
                std::cerr << "Entering command mode..." << std::endl;
                w->mCommandMode = true;
                w->mCommandBuff.clear();
                break;
            }
        default:
            break;
    }
}

static inline void handleEvent(Window *w, SDL_Event *ev, StateManager *sm) {
    const bool *key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_ESCAPE]) {
        w->mExit = true;
        return;
    }

    if (!w->mCommandMode) {
        static const int movementIncrement = (int)(MOVEMENT_INCREMENT * (sm->mActive->mZoom / 100));
        if (key_state[SDL_SCANCODE_H])
            sm->mActive->moveTexturePosBy(std::pair<int, int>(movementIncrement, 0));
        if (key_state[SDL_SCANCODE_J])
            sm->mActive->moveTexturePosBy(std::pair<int, int>(0, -movementIncrement));
        if (key_state[SDL_SCANCODE_K])
            sm->mActive->moveTexturePosBy(std::pair<int, int>(0, movementIncrement));
        if (key_state[SDL_SCANCODE_L])
            sm->mActive->moveTexturePosBy(std::pair<int, int>(-movementIncrement, 0));
    }

    while (SDL_PollEvent(ev)) {
        bool keyDown = false;
        switch (ev->type) {
            case SDL_EVENT_QUIT:
                w->mExit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                if (sm->mActive->mFitIn) sm->mActive->fitTextureToWindow();
                break;
            case SDL_EVENT_KEY_DOWN:
                keyDown = true;
                break;
            default:
                break;
        }

        if (!keyDown) continue;
        if (w->mCommandMode) handleKeyCommand(w, sm, ev);
        else                 handleKeyNormal(w, sm, ev);
    }
}

