#pragma once

#include "state.h"
#include <queue>
#include <vector>

class StateManager {
    public:
        State *mActive;
        int mActiveIdx;
        std::vector<State *> mStates;
        std::queue<State *> mQueue;

        StateManager();
        ~StateManager();

        bool activeteState(size_t idx);
        bool activeteState(const char *path);

        size_t addState(State *s);
        void deleteState(const char *path);
        void deleteState(size_t idx);
        int makeNewState(Window *w, const char *path, SDL_ScaleMode scaleMode);
        int makeNewState(Window *w, const char *path);

    private:
        State *_searchState(const char *path, size_t *idx);
};
