#pragma once

#include "state.h"
#include <queue>
#include <vector>
#include <atomic>

struct Window;

class StateManager {
    public:
        State *mActive;
        int mActiveIdx;
        std::vector<State *> mStates;
        std::atomic<bool> mRunning {true};

        // NOTE: Access using mutex!
        std::mutex mMutex;
        std::queue<State *> mQueue;

        StateManager();
        ~StateManager();

        bool activateState(size_t idx);
        bool activateState(const char *path);

        size_t addState(State *s);
        void deleteState(const char *path);
        void deleteState(size_t idx);
        int newState(Window *w, const char *path, SDL_ScaleMode scaleMode);
        int newState(Window *w, const char *path);

        void mainLoop();
        void stopLoop();

    private:
        State *_searchState(const char *path, int *idx);
};
